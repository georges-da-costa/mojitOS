This sensor can autodetect interfaces in use by giving the special
interface name "X".  But the total number of interfaces it can autodetect
is currently under a hard-limit.  This hard-limit can be changed by
modifying this line in `src/network.c`:

```c
#define NB_MAX_DEV 8
```
