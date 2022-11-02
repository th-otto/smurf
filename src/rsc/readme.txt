This directory only has the startup source, which is language
independant.

When changing resource files, make sure to update all languages
(currently en/de/fr), and that the structure is identical in all
resources (can easily be verified by doing a diff on the header files).
Also, when the structure changes, make sure to copy one of the header
files (preferably the english version) to this directory.
