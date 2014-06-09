#ifndef __AT24_H__
#define __AT24_H__

int at24_init(void);
ssize_t at24_read(char *buf, loff_t off, size_t cnt);
ssize_t at24_write(const char *buf, loff_t off, size_t cnt);
#endif
