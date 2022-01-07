ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = azu21

azu21_SOURCES = src/azu21.c \
  src/charger.c \
  src/charger.h \
  src/display.c \
  src/display.h \
  src/keyboard.c \
  src/keyboard.h \
  src/utils.c \
  src/utils.h \
  src/hal/startup_stm32f0xx.c \
  src/hal/system_stm32f0xx.c \
  src/hal/system_stm32f0xx.h \
  src/hal/systick.h \
  src/hal/systick.c

azu21_CFLAGS = -mcpu=cortex-m0 -mthumb -I$(srcdir)/include -I$(srcdir)/src -I$(srcdir)/src/hal --specs=nosys.specs -DSTM32F030x8 -DFREQUENCY=8000000
azu21_LDFLAGS = -L$(srcdir)/ld -Tstm32f030x.ld -nostartfiles -lnosys

chmod-bin :
	chmod +x $(builddir)/program.sh
	chmod +x $(builddir)/connect.sh
