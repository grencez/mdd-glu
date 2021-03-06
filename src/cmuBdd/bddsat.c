/* BDD satisfying valuation routines */


#include "bddint.h"

#ifdef STDC_HEADERS
#  include <stdlib.h>
#else
extern void qsort(pointer, unsigned long, unsigned long, int (*)(const void *, const void *));
#endif

static
bdd
cmu_bdd_satisfy_step(cmu_bdd_manager bddm, bdd f)
{
  bdd temp;
  bdd result;

  BDD_SETUP(f);
  if (BDD_IS_CONST(f))
    return (f);
  if (BDD_THEN(f) == BDD_ZERO(bddm))
    {
      temp=cmu_bdd_satisfy_step(bddm, BDD_ELSE(f));
      result=bdd_find(bddm, BDD_INDEXINDEX(f), BDD_ZERO(bddm), temp);
    }
  else
    {
      temp=cmu_bdd_satisfy_step(bddm, BDD_THEN(f));
      result=bdd_find(bddm, BDD_INDEXINDEX(f), temp, BDD_ZERO(bddm));
    }
  return (result);
}


/* cmu_bdd_satisfy(bddm, f) returns a BDD which implies f, is true for */
/* some valuation on which f is true, and which has at most one node */
/* at each level. */

bdd
cmu_bdd_satisfy(cmu_bdd_manager bddm, bdd f)
{
  if (bdd_check_arguments(1, f))
    {
      if (f == BDD_ZERO(bddm))
	{
	  cmu_bdd_warning("cmu_bdd_satisfy: argument is false");
	  return (f);
	}
      FIREWALL(bddm);
      RETURN_BDD(cmu_bdd_satisfy_step(bddm, f));
    }
  return ((bdd)0);
}


static
bdd
cmu_bdd_satisfy_support_step(cmu_bdd_manager bddm, bdd f, bdd_indexindex_type *support)
{
  bdd temp;
  bdd result;

  BDD_SETUP(f);
  if (!*support)
    return (cmu_bdd_satisfy_step(bddm, f));
  if (BDD_INDEX(bddm, f) <= bddm->indexes[*support])
    {
      if (BDD_INDEXINDEX(f) == *support)
	++support;
      if (BDD_THEN(f) == BDD_ZERO(bddm))
	{
	  temp=cmu_bdd_satisfy_support_step(bddm, BDD_ELSE(f), support);
	  result=bdd_find(bddm, BDD_INDEXINDEX(f), BDD_ZERO(bddm), temp);
	}
      else
	{
	  temp=cmu_bdd_satisfy_support_step(bddm, BDD_THEN(f), support);
	  result=bdd_find(bddm, BDD_INDEXINDEX(f), temp, BDD_ZERO(bddm));
	}
    }
  else
    {
      temp=cmu_bdd_satisfy_support_step(bddm, f, support+1);
      result=bdd_find(bddm, *support, BDD_ZERO(bddm), temp);
    }
  return (result);
}


static
int
index_cmp(pointer p1, pointer p2)
{
  bdd_index_type i1, i2;

  i1= *(bdd_indexindex_type *)p1;
  i2= *(bdd_indexindex_type *)p2;
  if (i1 < i2)
    return (-1);
  if (i1 > i2)
    return (1);
  return (0);
}


/* cmu_bdd_satisfy_support(bddm, f) returns a BDD which implies f, is true */
/* for some valuation on which f is true, which has at most one node */
/* at each level, and which has exactly one node corresponding to each */
/* variable which is associated with something in the current variable */
/* association. */

bdd
cmu_bdd_satisfy_support(cmu_bdd_manager bddm, bdd f)
{
  bdd_indexindex_type *support, *p;
  long i;
  bdd result;

  if (bdd_check_arguments(1, f))
    {
      if (f == BDD_ZERO(bddm))
	{
	  cmu_bdd_warning("cmu_bdd_satisfy_support: argument is false");
	  return (f);
	}
      support=(bdd_indexindex_type *)mem_get_block((bddm->vars+1)*sizeof(bdd));
      FIREWALL1(bddm,
		if (retcode == BDD_ABORTED || retcode == BDD_OVERFLOWED)
		  {
		    mem_free_block((pointer)support);
		    return ((bdd)0);
		  }
		);
      for (i=0, p=support; i < bddm->vars; ++i)
	if (bddm->curr_assoc->assoc[i+1])
	  {
	    *p=bddm->indexes[i+1];
	    ++p;
	  }
      *p=0;
      qsort((pointer)support, (unsigned)(p-support),
	    sizeof(bdd_indexindex_type),
	    (int (*)(const void *, const void *))index_cmp);
      while (p != support)
	{
	  --p;
	  *p=bddm->indexindexes[*p];
	}
      result=cmu_bdd_satisfy_support_step(bddm, f, support);
      mem_free_block((pointer)support);
      RETURN_BDD(result);
    }
  return ((bdd)0);
}


double
cmu_bdd_satisfying_fraction_step(cmu_bdd_manager bddm, bdd f)
{
  union {
    long cache_result[2];
    double result;
  } u;

  BDD_SETUP(f);
  if (BDD_IS_CONST(f))
    {
      if (f == BDD_ZERO(bddm))
	return (0.0);
      return (1.0);
    }
  if (bdd_lookup_in_cache1d(bddm, OP_SATFRAC, f,
			    &(u.cache_result[0]), &(u.cache_result[1])))
    {
      return (u.result);
    }
  u.result=0.5*cmu_bdd_satisfying_fraction_step(bddm, BDD_THEN(f))+
    0.5*cmu_bdd_satisfying_fraction_step(bddm, BDD_ELSE(f));
  bdd_insert_in_cache1d(bddm, OP_SATFRAC, f, u.cache_result[0],
			u.cache_result[1]);
  return (u.result);
}


/* cmu_bdd_satisfying_fraction(bddm, f) returns the fraction of valuations */
/* which make f true.  (Note that this fraction is independent of */
/* whatever set of variables f is supposed to be a function of.) */

double
cmu_bdd_satisfying_fraction(cmu_bdd_manager bddm, bdd f)
{
  if (bdd_check_arguments(1, f))
    return (cmu_bdd_satisfying_fraction_step(bddm, f));
  return (0.0);
}
