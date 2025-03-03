
/* Copyright 2018 Codership Oy <info@codership.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#include "mysql/components/services/log_builtins.h"
#include "mysql/psi/mysql_cond.h"
#include "mysql/psi/mysql_mutex.h"

#include "wsrep_client_service.h"
#include "wsrep_client_state.h"
#include "wsrep_high_priority_service.h"
#include "wsrep_server_service.h"
#include "wsrep_server_state.h"
#include "wsrep_storage_service.h"

#include "wsrep_mysqld.h"
#include "wsrep_schema.h"
#include "wsrep_sst.h"
#include "wsrep_thd.h"
#include "wsrep_utils.h"
#include "wsrep_xid.h"

#include "log.h"         /* sql_print_xxx() */
#include "sql_base.h"    /* close_thread_tables */
#include "sql_class.h"   /* system variables */
#include "transaction.h" /* trans_xxx */

static void init_service_thd(THD *thd, const char *thread_stack) {
  thd->thread_stack = thread_stack;
  thd->real_id = pthread_self();
  // thd->prior_thr_create_utime= thd->start_utime=
  // microsecond_interval_timer();
  thd->set_time();
  thd->set_command(COM_SLEEP);
  thd->reset_for_next_command();
}

wsrep::storage_service *Wsrep_server_service::storage_service(
    wsrep::client_service &client_service) {
  Wsrep_client_service &cs =
      static_cast<Wsrep_client_service &>(client_service);
  THD *thd = new THD(false, true);
  thd->set_new_thread_id();
  init_service_thd(thd, cs.m_thd->thread_stack);
  WSREP_DEBUG("Created storage service with thread id %u", thd->thread_id());
  /* Use variables from the current thd attached to client_service.
     This is because we need to be able to BF abort storage access
     operations. */
  wsrep_assign_from_threadvars(thd);
  return new Wsrep_storage_service(thd);
}

wsrep::storage_service *Wsrep_server_service::storage_service(
    wsrep::high_priority_service &high_priority_service) {
  Wsrep_high_priority_service &hps =
      static_cast<Wsrep_high_priority_service &>(high_priority_service);
  THD *thd = new THD(false, true);
  thd->set_new_thread_id();
  init_service_thd(thd, hps.m_thd->thread_stack);
  WSREP_DEBUG("Created high priority storage service with thread id %u",
              thd->thread_id());
  wsrep_assign_from_threadvars(thd);
  return new Wsrep_storage_service(thd);
}

void Wsrep_server_service::release_storage_service(
    wsrep::storage_service *storage_service) {
  Wsrep_storage_service *ss =
      static_cast<Wsrep_storage_service *>(storage_service);
  THD *thd = ss->m_thd;
  wsrep_reset_threadvars(thd);
  delete ss;
  delete thd;
}

Wsrep_applier_service *wsrep_create_streaming_applier(THD *orig_thd,
                                                      const char *ctx) {
  /* Reset variables to allow creating new variables in thread local
     storage for new THD if needed. Note that reset must be done for
     current_thd, as orig_thd may not be in effect. This may be the case when
     streaming transaction is BF aborted and streaming applier
     is created from BF aborter context. */
  Wsrep_threadvars saved_threadvars(wsrep_save_threadvars());
  wsrep_reset_threadvars(saved_threadvars.cur_thd);
  THD *thd = 0;
  Wsrep_applier_service *ret = 0;
  if (!wsrep_create_threadvars() && (thd = new THD(false, true))) {
    thd->set_new_thread_id();
    init_service_thd(thd, orig_thd->thread_stack);
    wsrep_assign_from_threadvars(thd);
    WSREP_DEBUG(
        "Created streaming applier service in %s context with "
        "thread id %u",
        ctx, thd->thread_id());
    if (!(ret = new (std::nothrow) Wsrep_applier_service(thd))) {
      delete thd;
    }
  }
  /* Restore original thread local storage state before returning. */
  wsrep_restore_threadvars(saved_threadvars);
  wsrep_store_threadvars(saved_threadvars.cur_thd);
  return ret;
}

wsrep::high_priority_service *Wsrep_server_service::streaming_applier_service(
    wsrep::client_service &orig_client_service) {
  Wsrep_client_service &orig_cs =
      static_cast<Wsrep_client_service &>(orig_client_service);
  return wsrep_create_streaming_applier(orig_cs.m_thd, "local");
}

wsrep::high_priority_service *Wsrep_server_service::streaming_applier_service(
    wsrep::high_priority_service &orig_high_priority_service) {
  Wsrep_high_priority_service &orig_hps(
      static_cast<Wsrep_high_priority_service &>(orig_high_priority_service));
  return wsrep_create_streaming_applier(orig_hps.m_thd, "high priority");
}

void Wsrep_server_service::release_high_priority_service(
    wsrep::high_priority_service *high_priority_service) {
  Wsrep_high_priority_service *hps =
      static_cast<Wsrep_high_priority_service *>(high_priority_service);
  THD *thd = hps->m_thd;
  delete hps;
  wsrep_store_threadvars(thd);
  delete thd;
  wsrep_delete_threadvars();
}

void Wsrep_server_service::background_rollback(
    wsrep::client_state &client_state) {
  Wsrep_client_state &cs = static_cast<Wsrep_client_state &>(client_state);
  wsrep_fire_rollbacker(cs.thd());
}

void Wsrep_server_service::bootstrap() {
  wsrep::log_info()
      << "Bootstrapping a new cluster, setting initial position to "
      << wsrep::gtid::undefined();
  wsrep_set_SE_checkpoint(wsrep::gtid::undefined());
}

void Wsrep_server_service::log_message(enum wsrep::log::level level,
                                       const char *message) {
  switch (level) {
    case wsrep::log::unknown:
      WSREP_DEBUG("unknown: %s", message);
      break;
    case wsrep::log::debug:
      // sql_print_information("debug: %s", message);
      WSREP_DEBUG("debug: %s", message);
      break;
    case wsrep::log::info:
      // sql_print_information("%s", message);
      WSREP_INFO("%s", message);
      break;
    case wsrep::log::warning:
      // sql_print_warning("%s", message);
      WSREP_WARN("%s", message);
      break;
    case wsrep::log::error:
      // sql_print_error("%s", message);
      WSREP_ERROR("%s", message);
      break;
  }
}

void Wsrep_server_service::log_view(
    wsrep::high_priority_service *high_priority_service,
    const wsrep::view &view) {
  Wsrep_high_priority_service *applier =
      static_cast<Wsrep_high_priority_service *>(high_priority_service);
  /* Update global system variables */
  mysql_mutex_lock(&LOCK_global_system_variables);
  if (wsrep_auto_increment_control && view.own_index() >= 0) {
    global_system_variables.auto_increment_offset = view.own_index() + 1;
    global_system_variables.auto_increment_increment = view.members().size();
  }
  wsrep_protocol_version = view.protocol_version();
  bool not_shutdown = (pxc_maint_mode != PXC_MAINT_MODE_SHUTDOWN);
  bool multi_version_cluster = wsrep_protocol_version < 4;
  if (not_shutdown &&
      ((multi_version_cluster &&
        (pxc_strict_mode > PXC_STRICT_MODE_PERMISSIVE)) ||
       DBUG_EVALUATE_IF("simulate_wsrep_multiple_major_versions", true,
                        false))) {
    std::ostringstream os;
    os << "Detected Protocol version: " << wsrep_protocol_version
       << " Changing pxc_maint_mode to "
          "MAINTENANCE.";
    WSREP_INFO("%s", os.str().c_str());
    pxc_maint_mode = PXC_MAINT_MODE_MAINTENANCE;
    wsrep_pxc_maint_mode_forced = true;
  } else {
    /* if pxc_maint_mode was previously set by wsrep, we should reset */
    if (wsrep_pxc_maint_mode_forced &&
        pxc_maint_mode != PXC_MAINT_MODE_SHUTDOWN) {
      std::ostringstream os;
      os << "Detected Protocol version: " << wsrep_protocol_version
         << " Changing pxc_maint_mode to "
            "DISABLED.";
      WSREP_INFO("%s", os.str().c_str());
      pxc_maint_mode = PXC_MAINT_MODE_DISABLED;
      wsrep_pxc_maint_mode_forced = false;
    }
  }
  mysql_mutex_unlock(&LOCK_global_system_variables);

  /* Update wsrep status variables */
  mysql_mutex_lock(&LOCK_status);
  wsrep_cluster_size = view.members().size();
  wsrep_local_index = view.own_index();
  wsrep_cluster_conf_id = view.view_seqno().get();
  std::ostringstream os;
  os << view.state_id().id();
  wsrep_update_cluster_state_uuid(os.str().c_str());
  mysql_mutex_unlock(&LOCK_status);
  local_status.set(local_status.get(), &view);

  if (view.status() == wsrep::view::primary) {
    if (applier) {
      Wsrep_id id;
      Wsrep_view prev_view = wsrep_schema->restore_view(applier->m_thd, id);
      bool checkpoint_was_reset = false;
      if (prev_view.state_id().id() != view.state_id().id()) {
        WSREP_DEBUG("New cluster UUID was generated, resetting position info");
        wsrep_set_SE_checkpoint(wsrep::gtid::undefined());
        checkpoint_was_reset = true;
      }

      if (wsrep_debug) {
        std::ostringstream os2;
        os2 << "Storing cluster view:\n" << view;
        WSREP_INFO("%s", os2.str().c_str());
        assert(prev_view.state_id().id() != view.state_id().id() ||
                    view.state_id().seqno().get() >=
                        prev_view.state_id().seqno().get());
      }

      if (trans_begin(applier->m_thd, MYSQL_START_TRANS_OPT_READ_WRITE)) {
        WSREP_WARN("Failed to start transaction for store view");
      } else {
        if (wsrep_schema->store_view(applier->m_thd, view)) {
          WSREP_WARN("Failed to store view");
          trans_rollback_stmt(applier->m_thd);
          if (!trans_rollback(applier->m_thd)) {
            close_thread_tables(applier->m_thd);
          }
        } else {
          if (trans_commit(applier->m_thd)) {
            WSREP_WARN("Failed to commit transaction for store view");
          }
        }
        applier->m_thd->mdl_context.release_transactional_locks();
      }

      /*
        Backwards compatibility: When running in mixed cluster with
        Galera 3.x, the provider does not generate unique sequence numbers
        for views. This condition can be checked by inspecting last
        committed as returned by the provider. If the last_committed
        matches to view state_id seqno, the cluster runs in backwards
        compatibility mode and we skip setting the checkpoint for
        view.
      */
      wsrep::seqno last_committed = Wsrep_server_state::instance()
                                        .provider()
                                        .last_committed_gtid()
                                        .seqno();
      if (checkpoint_was_reset || last_committed != view.state_id().seqno()) {
        wsrep_set_SE_checkpoint(view.state_id());
      }
      assert(wsrep_get_SE_checkpoint().id() == view.state_id().id());
    } else {
      WSREP_DEBUG(
          "No applier in Wsrep_server_service::log_view(), "
          "skipping write to wsrep_schema");
    }
  }
}

void Wsrep_server_service::recover_streaming_appliers(
    wsrep::client_service &cs) {
  Wsrep_client_service &client_service =
      static_cast<Wsrep_client_service &>(cs);
  wsrep_recover_sr_from_storage(client_service.m_thd);
}

void Wsrep_server_service::recover_streaming_appliers(
    wsrep::high_priority_service &hs) {
  Wsrep_high_priority_service &high_priority_service =
      static_cast<Wsrep_high_priority_service &>(hs);
  wsrep_recover_sr_from_storage(high_priority_service.m_thd);
}

wsrep::view Wsrep_server_service::get_view(wsrep::client_service &c,
                                           const wsrep::id &own_id) {
  Wsrep_client_service &cs(static_cast<Wsrep_client_service &>(c));
  wsrep::view v(wsrep_schema->restore_view(cs.m_thd, own_id));
  return v;
}

wsrep::gtid Wsrep_server_service::get_position(wsrep::client_service &) {
  return wsrep_get_SE_checkpoint();
}

void Wsrep_server_service::set_position(wsrep::client_service& c WSREP_UNUSED,
                                        const wsrep::gtid& gtid)
{
  Wsrep_client_service& cs WSREP_UNUSED (static_cast<Wsrep_client_service&>(c));
  assert(cs.m_client_state.transaction().state()
              == wsrep::transaction::s_aborted);
 // Wait until all prior committers have finished.
  wsrep::gtid wait_for(gtid.id(),
                       wsrep::seqno(gtid.seqno().get() - 1));
  if (auto err = Wsrep_server_state::instance().provider()
      .wait_for_gtid(wait_for, std::numeric_limits<int>::max()))
  {
    WSREP_WARN("Wait for gtid returned error %d while waiting for "
               "prior transactions to commit before setting position", err);
  }
  wsrep_set_SE_checkpoint(gtid);
}

void Wsrep_server_service::log_state_change(
    enum Wsrep_server_state::state prev_state,
    enum Wsrep_server_state::state current_state) {
  WSREP_INFO("Server status change %s -> %s", wsrep::to_c_string(prev_state),
             wsrep::to_c_string(current_state));
  mysql_mutex_lock(&LOCK_status);
  switch (current_state) {
    case Wsrep_server_state::s_synced:
      wsrep_ready = true;
      WSREP_INFO("Synchronized with group, ready for connections");
      [[fallthrough]];
    case Wsrep_server_state::s_joined:
      [[fallthrough]];
    case Wsrep_server_state::s_donor:
      wsrep_cluster_status = "Primary";
      break;
    case Wsrep_server_state::s_connected:
      wsrep_cluster_status = "non-Primary";
      wsrep_ready = false;
      wsrep_connected = true;
      break;
    case Wsrep_server_state::s_disconnected:
      wsrep_ready = false;
      wsrep_connected = false;
      wsrep_cluster_status = "Disconnected";
      break;
    default:
      wsrep_ready = false;
      wsrep_cluster_status = "non-Primary";
      break;
  }
  mysql_mutex_unlock(&LOCK_status);
  local_status.set(current_state);
}

bool Wsrep_server_service::sst_before_init() const { return wsrep_before_SE(); }

std::string Wsrep_server_service::sst_request() { return wsrep_sst_prepare(); }

int Wsrep_server_service::start_sst(const std::string &sst_request,
                                    const wsrep::gtid &gtid, bool bypass) {
  return wsrep_sst_donate(sst_request, gtid, bypass);
}

int Wsrep_server_service::wait_committing_transactions(int timeout) {
  return wsrep_wait_committing_connections_close(timeout);
}

void Wsrep_server_service::debug_sync(const char *) {}
