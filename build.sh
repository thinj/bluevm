#!/bin/sh


SRC1="console.c constantpool.c debugger.c disassembler.c frame.c heap.c instruction1.c" 
SRC2="Java_java_lang_Object.c Java_java_lang_Class.c main.c operandstack.c xyprintf.c nostdlib.c"
SRC3="Java_java_io_PrintStream.c" 
SRC4="trace.c jni.c Java_java_lang_Throwable.c" 
SRC="$SRC1 $SRC2 $SRC3 $SRC4"

INC1="console.h constantpool.h debugger.h disassembler.h frame.h heap.h instructions.h" 
INC2="jni.h operandstack.h types.h xyprintf.h hardware.h trace.h davsdu.h" 
INC="$INC1 $INC2"


SRCDIR=../thinjvm
DSTDIR=.

##################################################3

if [ ! -d $SRCDIR ]; then
   echo "No source dir: $SRCDIR" 
   exit 1
fi    

if [ ! -d $DSTDIR ]; then
   echo "No destination dir: $DSTDIR" 
   exit 1
fi    

for file in $SRC 
do
   base=`basename $file .c`
   if [ -e $base.o ]; then 
       if [ $base.o -ot ../thinjvm/$base.c ]; then
           echo "Copying $base.c ..."
           cp $SRCDIR/$base.c $DSTDIR
       fi
   else
       echo "Copying $base.c ..."
       cp $SRCDIR/$base.c $DSTDIR
   fi  
done

for file in $INC 
do
    cp $SRCDIR/$file $DSTDIR
done


make

for file in $SRC 
do
   if [ -e $file ]; then
       echo "Cleaning up $file ..."
       rm $file 
   fi  
done

#for file in $INC 
#do
#    rm $DSTDIR/$file
#done



exit

##########################

# cp ../thinjvm/blueboard.c .
cp ../thinjvm/console.c .
cp ../thinjvm/constantpool.c .
# cp ../thinjvm/davsdu.c .
cp ../thinjvm/debugger.c .
cp ../thinjvm/disassembler.c .
cp ../thinjvm/frame.c .
cp ../thinjvm/heap.c .
cp ../thinjvm/instruction1.c .

cp ../thinjvm/Java_java_lang_Object.c .
cp ../thinjvm/Java_java_lang_Class.c .
cp ../thinjvm/Java_thinj_regression_Regression.c .
cp ../thinjvm/Java_thinj_regression_TinySystem.c .

cp ../thinjvm/main.c .
# cp ../thinjvm/meggyjr.c .
cp ../thinjvm/operandstack.c .
cp ../thinjvm/xyprintf.c .
# cp ../thinjvm/config.h .
cp ../thinjvm/console.h .
cp ../thinjvm/constantpool.h .
cp ../thinjvm/debugger.h .
cp ../thinjvm/disassembler.h .
cp ../thinjvm/frame.h .
cp ../thinjvm/heap.h .
cp ../thinjvm/instructions.h .
cp ../thinjvm/jni.h .
cp ../thinjvm/operandstack.h .
cp ../thinjvm/types.h .
cp ../thinjvm/xyprintf.h .
