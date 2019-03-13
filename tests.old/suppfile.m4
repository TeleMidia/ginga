dnl suppfile.m4 -- Generate Valgrind suppressions.
changequote([,])dnl
define([suppress],[dnl
{
    $1: $2
    Memcheck:Leak
    ...
    fun:$1
}])dnl

dnl Linker.
suppress(_dl_init,                          System linker)

dnl GObject.
suppress(g_type_register_fundamental,       GObject type system)
suppress(g_type_register_static,            GObject type system)
