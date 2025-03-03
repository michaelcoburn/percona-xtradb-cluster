# -*- coding: utf-8 -*-
#
# Percona XtraDB Cluster documentation build configuration file, created by
# sphinx-quickstart on Mon Jun 27 22:27:15 2011.
#
# This file is execfile()d with the current directory set to its containing dir.
#
# Note that not all possible configuration values are present in this
# autogenerated file.
#
# All configuration values have a default; values that are commented out
# serve to show the default.

import sys, os

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#sys.path.insert(0, os.path.abspath('.'))

sys.path.append(os.path.abspath('ext'))

# -- General configuration -----------------------------------------------------

# If your documentation needs a minimal Sphinx version, state it here.
#needs_sphinx = '1.0'

# Add any Sphinx extension module names here, as strings. They can be extensions
# coming with Sphinx (named 'sphinx.ext.*') or your custom ones.
extensions = ['sphinx.ext.intersphinx', 'sphinx.ext.todo',
              'sphinx.ext.coverage', 'sphinx.ext.ifconfig',
              'sphinx.ext.extlinks', 'psdom']

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# The suffix of source filenames.
source_suffix = '.rst'

# The encoding of source files.
#source_encoding = 'utf-8-sig'

# The master toctree document.
master_doc = 'index'

# General information about the project.
project = u'Percona XtraDB Cluster'
copyright = u'Percona LLC and/or its affiliates 2009-2022'

# The version info for the project you're documenting, acts as replacement for
# |version| and |release|, also used in various other places throughout the
# built documents.
#
# The short X.Y version.
version = '8.0'
# The full version, including alpha/beta/rc tags.
release = '8.0.27-17.1'
# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#language = None

# There are two options for replacing |today|: either, you set today to some
# non-false value, then it is used:
#today = ''
# Else, today_fmt is used as the format for a strftime call.
#today_fmt = '%B %d, %Y'

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
exclude_patterns = []

# The reST default role (used for this markup: `text`) to use for all documents.
#default_role = None

primary_domain = 'psdom'

# If true, '()' will be appended to :func: etc. cross-reference text.
#add_function_parentheses = True

# If true, the current module name will be prepended to all description
# unit titles (such as .. function::).
#add_module_names = True

# If true, sectionauthor and moduleauthor directives will be shown in the
# output. They are ignored by default.
#show_authors = False

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'


rst_prolog = '''

.. _Galera: https://github.com/percona/galera

.. _MariaDB: https://www.mariadb.com/

.. _MySQL: https://www.mysql.com/

.. |Percona Server| replace:: Percona Server

.. |TokuDB| replace:: TokuDB

.. |PMM| replace:: Percona Monitoring and Management

.. _PMM: https://www.percona.com/software/database-tools/percona-monitoring-and-management

.. _ProxySQL: http://www.proxysql.com/

.. |PS| replace:: Percona Server

.. _PS: https://www.percona.com/software/mysql-database/percona-server

.. |PXB| replace:: Percona XtraBackup

.. _PXB: https://www.percona.com/software/mysql-database/percona-xtrabackup

.. |PXC| replace:: Percona XtraDB Cluster

.. _PXC: https://www.percona.com/software/mysql-database/percona-xtradb-cluster

.. _XtraDB: https://www.percona.com/software/mysql-database/percona-server/xtradb

.. |check|  replace:: ``|[[---CHECK---]]|``

.. |xtrabackup|  replace:: :program:`xtrabackup`

.. |innobackupex|  replace:: :program:`innobackupex`

.. |XtraDB|  replace:: *XtraDB*

.. |IST|  replace:: :term:`IST`

.. |SST|  replace:: :term:`SST`

.. |XtraDB Cluster|  replace:: :term:`XtraDB Cluster`

.. |Percona XtraDB Cluster|  replace:: *Percona XtraDB Cluster*

.. |InnoDB|  replace:: *InnoDB*

.. |HAProxy|  replace:: :term:`HAProxy`

.. |MyISAM|  replace:: *MyISAM*

.. |split brain|  replace:: :term:`split brain`

.. |.frm|  replace:: :term:`.frm`

.. |LSN|  replace:: :term:`LSN`

.. |XtraBackup|  replace:: *XtraBackup*

.. |Percona XtraBackup|  replace:: *Percona XtraBackup*

.. |Percona|  replace:: *Percona*

.. |MySQL|  replace:: *MySQL*

.. |sysbench|  replace:: *sysbench*

.. |Drizzle|  replace:: *Drizzle*

.. |tar4ibd|  replace:: :program:`tar4ibd`

.. |tar|  replace:: :program:`tar`

'''

extlinks = {
  'psbug': ('https://jira.percona.com/browse/PS-%s', '#'),
  'pxcbug': ('https://jira.percona.com/browse/PXC-%s', '#'),
  'tdbbug': ('https://jira.percona.com/browse/TDB-%s', '#'),
  'bug': ('https://bugs.launchpad.net/percona-xtradb-cluster/+bug/%s', '#'),
  'mysqlbug': ('http://bugs.mysql.com/bug.php?id=%s', '#'),
  'githubbug': ('https://github.com/codership/galera/issues/%s', '#'),
  'wsrepbug': ('https://github.com/codership/mysql-wsrep/issues/%s', '#'),
  'jirabug': ('https://jira.percona.com/browse/%s', ''),
  'mdevbug': ('https://jira.mariadb.org/browse/MDEV-%s', '#')
}

# A list of ignored prefixes for module index sorting.
#modindex_common_prefix = []


# -- Options for HTML output ---------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
html_theme = 'percona-theme'

# Theme options are theme-specific and customize the look and feel of a theme
# further.  For a list of options available for each theme, see the
# documentation.
#html_theme_options = {}

# Add any paths that contain custom themes here, relative to this directory.
html_theme_path = ['.', './percona-theme']

# The name for this set of Sphinx documents.  If None, it defaults to
# "<project> v<release> documentation".
html_title = 'Percona XtraDB Cluster Documentation'

# A shorter title for the navigation bar.  Default is the same as html_title.
html_short_title = 'Percona XtraDB Cluster'

# The name of an image file (relative to this directory) to place at the top
# of the sidebar.
#html_logo = 'percona-xtrabackup-logo.jpg'
html_logo = ''

# The name of an image file (within the static path) to use as favicon of the
# docs.  This file should be a Windows icon file (.ico) being 16x16 or 32x32
# pixels large.
html_favicon = 'percona_favicon.ico'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# If not '', a 'Last updated on:' timestamp is inserted at every page bottom,
# using the given strftime format.
#html_last_updated_fmt = '%b %d, %Y'

# If true, SmartyPants will be used to convert quotes and dashes to
# typographically correct entities.
#html_use_smartypants = True

# Custom sidebar templates, maps document names to template names.
#html_sidebars = {}
html_sidebars = {
        '**': ['localtoc.html', 'sourcelink.html'],
        'using/windows': ['windowssidebar.html'],
}

# Additional templates that should be rendered to pages, maps page names to
# template names.
#html_additional_pages = {}

# If false, no module index is generated.
#html_domain_indices = True

# If false, no index is generated.
#html_use_index = True

# If true, the index is split into individual pages for each letter.
#html_split_index = False

# If true, links to the reST sources are added to the pages.
#html_show_sourcelink = True

# If true, "Created using Sphinx" is shown in the HTML footer. Default is True.
#html_show_sphinx = True

# If true, "(C) Copyright ..." is shown in the HTML footer. Default is True.
#html_show_copyright = True

# If true, an OpenSearch description file will be output, and all pages will
# contain a <link> tag referring to it.  The value of this option must be the
# base URL from which the finished HTML is served.
#html_use_opensearch = ''

# This is the file name suffix for HTML files (e.g. ".xhtml").
#html_file_suffix = None

# Output file base name for HTML help builder.
htmlhelp_basename = 'PerconaXtraDBClusterDoc'


# -- Options for LaTeX output --------------------------------------------------

# The paper size ('letter' or 'a4').
#latex_paper_size = 'letter'

# The font size ('10pt', '11pt' or '12pt').
#latex_font_size = '10pt'

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title, author, documentclass [howto/manual]).
latex_documents = [
  ('index', 'PerconaXtraDBCluster-8.0.tex', u'Percona XtraDB Cluster Documentation',
   u'Percona LLC and/or its affiliates 2009-2022', 'manual'),
]

# The name of an image file (relative to this directory) to place at the top of
# the title page.
latex_logo = 'percona-logo-color.png'

# For "manual" documents, if this is true, then toplevel headings are parts,
# not chapters.
#latex_use_parts = False
latex_toplevel_sectioning = 'part'

# If true, show page references after internal links.
#latex_show_pagerefs = False

# If true, show URL addresses after external links.
#latex_show_urls = False

# Additional stuff for the LaTeX preamble.
#latex_preamble = ''

# Documents to append as an appendix to all manuals.
#latex_appendices = []

# If false, no module index is generated.
#latex_domain_indices = True


# -- Options for manual page output --------------------------------------------

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).

# Tip: To see the value assigned to the following variable, copy the
# expression to be assigned and evaluate it in Python3 schell.

man_pages = [('index', # source start file
              ''.join([''.join(project.lower().split()),
                       ''.join(version.split('.'))]), # name, such as perconaserver57
              ' '.join([project, version, u'Documentation']), # description, such as Percona Server 5.7 Documentation
              [u'Percona LLC and/or its affiliates 2009-2022'], # authors
              1 # manual section
)]

# Example configuration for intersphinx: refer to the Python standard library.
#intersphinx_mapping = {
#    'ps51' : ('https://www.percona.com/doc/percona-server/5.1/', None),
#    'ps55' : ('https://www.percona.com/doc/percona-server/5.5/', None),
#    'ps56' : ('https://www.percona.com/doc/percona-server/5.6/', None),
#    'xb21' : ('https://www.percona.com/doc/percona-xtrabackup/2.1/', None),
#    'ptoolkit' : ('https://www.percona.com/doc/percona-toolkit/2.2/', None)
# }
