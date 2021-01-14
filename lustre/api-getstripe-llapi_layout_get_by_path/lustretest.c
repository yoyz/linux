#include <errno.h>
#include <sys/vfs.h>
#include <lustre/lustreapi.h>

#define LOV_MAGIC_MAGIC 0x0BD0
#define LOV_MAGIC_MASK  0xFFFF

#define LOV_MAGIC_V1            (0x0BD10000 | LOV_MAGIC_MAGIC)
#define LOV_MAGIC_JOIN_V1       (0x0BD20000 | LOV_MAGIC_MAGIC)
#define LOV_MAGIC_V3            (0x0BD30000 | LOV_MAGIC_MAGIC)


static inline int maxint(int a, int b)
{
  return a > b ? a : b;
}

static void *alloc_lum()
{
  int v1, v3, join;
  
  v1 = sizeof(struct lov_user_md_v1) +
    LOV_MAX_STRIPE_COUNT * sizeof(struct lov_user_ost_data_v1);
  v3 = sizeof(struct lov_user_md_v3) +
    LOV_MAX_STRIPE_COUNT * sizeof(struct lov_user_ost_data_v1);
  
  return malloc(maxint(v1, v3));
}

int main(int argc, char** argv)
{
  struct lov_user_md *lum_file = NULL;
  int rc;
  int lum_size;
  uint64_t count;
  uint64_t size;
  uint32_t comp_id=1;
   

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return 1;
  }
  
  lum_file = alloc_lum();
  if (lum_file == NULL) {
    rc = ENOMEM;
    goto cleanup;
  }
  
  rc = llapi_file_get_stripe(argv[1], lum_file);
  if (rc) {
    rc = errno;
    goto cleanup;
  }

  /* stripe_size stripe_count */
  if (lum_file->lmm_magic == LOV_MAGIC_V1)
    {
      printf("lmm_magic:v1\n");
    }
  if (lum_file->lmm_magic == LOV_MAGIC_V3)
    {
      printf("lmm_magic: v3\n");
    }
  printf("stripe size:%d\nstipecount:%d\n",
	 lum_file->lmm_stripe_size,
	 lum_file->lmm_stripe_count);
  
  printf("\nWith the new api\n\n");
 
  struct llapi_layout * layout = llapi_layout_get_by_path(argv[1], 0);
  struct llapi_layout * comp=layout;
  
  
  /* Traverse the layout */
  rc = llapi_layout_comp_use(layout, LLAPI_LAYOUT_COMP_USE_FIRST);
  
  rc = llapi_layout_comp_id_get(comp,&comp_id);
  printf("comp id: %d\n", comp_id);
  
  rc = llapi_layout_stripe_count_get(comp, &count);
  printf("stripe_count: %llu\n", count);
  
  /* Get & print stripe size */
  rc = llapi_layout_stripe_size_get(comp, &size);
  printf("stripe_size: %llu\n", size);
  
  rc = llapi_layout_comp_use(layout, LLAPI_LAYOUT_COMP_USE_NEXT);
  rc = llapi_layout_comp_id_get(comp,&comp_id);
  printf("comp id: %d\n", comp_id);
  
  rc = llapi_layout_stripe_count_get(comp, &count);
  printf("stripe_count: %llu\n", count);
  
  /* Get & print stripe size */
  rc = llapi_layout_stripe_size_get(comp, &size);
  printf("stripe_size: %llu\n", size);
  
  rc = llapi_layout_comp_use(layout, LLAPI_LAYOUT_COMP_USE_NEXT);
  rc = llapi_layout_comp_id_get(comp,&comp_id);
  printf("comp id: %d\n", comp_id);
  
  rc = llapi_layout_stripe_count_get(comp, &count);
  printf("stripe_count: %llu\n", count);
  
  /* Get & print stripe size */
  rc = llapi_layout_stripe_size_get(comp, &size);
  printf("stripe_size: %llu\n", size);
    

cleanup:
  if (lum_file != NULL)
    free(lum_file);
  if (comp)
    free(comp);
	
  return rc;
}

