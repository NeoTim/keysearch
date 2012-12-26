/* © Torbjörn Pettersson 2007*/

#define _GNU_SOURCE

#define __KERNEL__ /* Only needed to enable some kernel-related defines */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <linux/types.h>

#ifdef CONFIG_LBDAF
   typedef unsigned long long sector_t;
#else
   typedef unsigned long sector_t;
#endif

/* Vanilla Linux Kernel 3.7 */
struct crypt_config
{
   struct dm_dev *dev;
   sector_t start;
   void *io_pool;
   void *req_pool;
   void *page_pool;
   void *bs;

   void *io_queue;
   void *crypt_queue;

   /* crypto related data */
   char *cipher;
   char *cipher_mode;

   struct crypt_iv_operations *iv_gen_ops;
   union {
       struct {
           void *hash_tfm;
           unsigned char *salt;
       } essiv;
       struct {
           int shift;
       } benbi;
   } iv_gen_private;
   sector_t iv_offset;
   unsigned int iv_size;

   char WTF[4];
   void *cpu; /* this field is specially aligned */

   void *iv_private;
   void *tfmsptr;

   unsigned tfms_count;
   unsigned int dmreq_start;
   unsigned long flags;
   unsigned int key_size;
   unsigned int key_parts;
   unsigned char key[0];
};

int keysearch(char *mem, int size)
{
   int i,j;
   struct crypt_config *cr;

   for(i = 0; i < (size - sizeof(struct crypt_config)); i++,mem++)
     {
        cr = (struct crypt_config *) mem;

        if((void *) cr->dev            > (void *) 0xffff800000000000 &&
           (void *) cr->io_pool        > (void *) 0xffff800000000000 &&
           (void *) cr->req_pool       > (void *) 0xffff800000000000 &&
           (void *) cr->page_pool      > (void *) 0xffff800000000000 &&
           (void *) cr->bs             > (void *) 0xffff800000000000 &&
           (void *) cr->io_queue       > (void *) 0xffff800000000000 &&
           (void *) cr->crypt_queue    > (void *) 0xffff800000000000 &&
           (void *) cr->cipher         > (void *) 0xffff800000000000 &&
           (void *) cr->cipher_mode    > (void *) 0xffff800000000000 &&
           (void *) cr->iv_gen_private.essiv.hash_tfm > (void *) 0xffff800000000000 &&
           cr->iv_offset == 0 &&
           (cr->iv_size  == 16 || cr->iv_size  == 32) &&
           (cr->key_size == 16 || cr->key_size == 32 || cr->key_size == 64)) {
             if(cr->start > 0)
               printf("offset: %lu blocks\n",
                      (unsigned long int ) cr->start);
             printf("iv_size : %d\n", cr->iv_size);
             printf("keylength: %d\n",(cr->key_size * 8));
             printf("keyparts: %d\n", cr->key_parts);
             printf("key: ");
             for(j = 0; j < cr->key_size; j++)
               printf("%02X",cr->key[j]);
             printf("\n");
             /* printf("flags : %d\n", cr->flags);
             printf("start : %p\n", cr->start);
             printf("start : %d\n", cr->dmreq_start);
             printf("count : %d\n", cr->tfms_count);
             printf("%p\n", cr->tfmsptr);
             printf("private %p\n", cr->iv_private);
             printf("cpu %p\n", cr->cpu);
             printf("dev %p\n", cr->dev);
             printf("genops %p\n", cr->iv_gen_ops);
             printf("%p\n", cr->cipher); */
       }
     }
   return(0);
}

int main(int argc, char **argv)
{
   int fd;
   char *mem = NULL;
   struct stat st;

   if(argc < 2)
     {
        printf("Usage: %s [memory dump file]\n",argv[0]);
        exit(-1);
     }

   if(stat(argv[1],&st) == -1)
     {
        perror("stat()");
        printf("Failed to stat %s\n",argv[1]);
        exit(-1);
     }

   fd = open(argv[1],O_RDONLY);
   if(fd == -1)
     {
        perror("open()");
        printf("Failed to open %s\n",argv[1]);
        exit(-1);
     }

   mem = mmap(0,(int)st.st_size, PROT_READ, MAP_SHARED, fd, 0);
   if(mem == ((void *) -1))
     {
        perror("mmap()");
        exit(-1);
     }

   (void)keysearch(mem,(int)st.st_size);
   return(0);
}
