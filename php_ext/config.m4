dnl $Id$
dnl config.m4 for extension ppc_coreimgclip

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(ppc_coreimgclip, for ppc_coreimgclip support,
Make sure that the comment is aligned:
[  --with-ppc_coreimgclip             Include ppc_coreimgclip support])

dnl Otherwise use enable:

if test "$PHP_PPC_COREIMGCLIP" != "no"; then

  dnl # --with-ppc_coreimgclip -> check with-path
  SEARCH_PATH="/usr/lib/pkgconfig /usr/local/lib/pkgconfig"     # you might want to change this
  SEARCH_FOR="opencv.pc"  # you most likely want to change this
  if test -r $PHP_PPC_COREIMGCLIP/$SEARCH_FOR; then # path given as parameter
     PPC_COREIMGCLIP_DIR=$PHP_PPC_COREIMGCLIP
  else # search default path list
     AC_MSG_CHECKING([for opencv.pc file in default path])
     for i in $SEARCH_PATH ; do
       if test -r $i/$SEARCH_FOR; then
         PPC_COREIMGCLIP_DIR=$i
         AC_MSG_RESULT(found in $i)
       fi
     done
  fi
  dnl
  if test -z "$PPC_COREIMGCLIP_DIR"; then
     AC_MSG_RESULT([not found])
     AC_MSG_ERROR([does not find opencv.pc])
  fi

  OPENCV_FLAGS="`pkg-config opencv --libs --cflags opencv`"
  opencv_lib_path=""
  opencv_last_path=""
  for i in $OPENCV_FLAGS;do
        if test ${i:0:2} = "-I" ;then
                opencv_last_path=$i
                PHP_ADD_INCLUDE(${i:2})
        elif test ${i:0:2} = "-L" ;then
                opencv_last_path=$i
                opencv_lib_path=${i:2}
        elif test ${i:${#i}-3} = ".so" ;then
                opencv_last_path=$i
                dir_name=`dirname $i`
                file_name=${i/$dir_name/}
                file_name=${file_name/\/lib/}
                file_name=${file_name/.so/}
		        PHP_ADD_LIBRARY_WITH_PATH($file_name,$dir_name,PPC_COREIMGCLIP_SHARED_LIBADD)
        elif test ${i:${#i}-6} = ".dylib" ;then
                opencv_last_path=$i
                dir_name=`dirname $i`
                file_name=${i/$dir_name/}
                file_name=${file_name/\/lib/}
                file_name=${file_name/.dylib/}
		        PHP_ADD_LIBRARY_WITH_PATH($file_name,$dir_name,PPC_COREIMGCLIP_SHARED_LIBADD)
        elif test ${i:0:2} = "-l" ;then 
                opencv_last_path=$i
                file_name=${i:2}
                PHP_ADD_LIBRARY_WITH_PATH($file_name,$opencv_lib_path,PPC_COREIMGCLIP_SHARED_LIBADD)
        fi
  done

  if test -z $opencv_last_path ;then
        AC_MSG_ERROR([no result from pkg-config opencv --libs --cflags opencv])
  fi

  PHP_ADD_LIBRARY(stdc++,"",PPC_COREIMGCLIP_SHARED_LIBADD)
  PHP_SUBST(PPC_COREIMGCLIP_SHARED_LIBADD)
  PHP_REQUIRE_CXX()

  PHP_NEW_EXTENSION(ppc_coreimgclip, ppc_coreimgclip.cpp, $ext_shared)
fi
