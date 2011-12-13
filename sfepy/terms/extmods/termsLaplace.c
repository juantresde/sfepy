#include "termsLaplace.h"
#include "terms.h"

#undef __FUNC__
#define __FUNC__ "laplace_build_gtg"
/*!
  @par Revision history:
  - 28.11.2005, c
  - 30.05.2007
*/
int32 laplace_build_gtg( FMField *out, FMField *gc )
{
  int32 iqp, ir, ic, nEP, nQP, nCol;
  float64 *pout, *pg1, *pg2, *pg3;

  nEP = gc->nCol;
  nQP = gc->nLev;
  nCol = out->nCol;

  fmf_fillC( out, 0.0 );
  switch (gc->nRow) {
  case 3:
    for (iqp = 0; iqp < nQP; iqp++) {
      pg1 = FMF_PtrLevel( gc, iqp );
      pg2 = pg1 + nEP;
      pg3 = pg2 + nEP;

      pout = FMF_PtrLevel( out, iqp );

      for (ir = 0; ir < nEP; ir++) {
	for (ic = 0; ic < nEP; ic++) {
	  pout[ic] = pg1[ir] * pg1[ic] + pg2[ir] * pg2[ic] + pg3[ir] * pg3[ic];
	}
	pout += nCol;
      }
    }
    break;
    
  case 2:
    for (iqp = 0; iqp < nQP; iqp++) {
      pg1 = FMF_PtrLevel( gc, iqp );
      pg2 = pg1 + nEP;

      pout = FMF_PtrLevel( out, iqp );

      for (ir = 0; ir < nEP; ir++) {
	for (ic = 0; ic < nEP; ic++) {
	  pout[ic] = pg1[ir] * pg1[ic] + pg2[ir] * pg2[ic];
	}
	pout += nCol;
      }
    }
    break;
    
  default:
    errput( ErrHead "ERR_Switch\n" );
    return( RET_Fail );
  }    
  return( RET_OK );
}


#undef __FUNC__
#define __FUNC__ "laplace_act_g_m"
/*!
  @par Revision history:
  - 28.11.2005, c
  - 30.05.2007
*/
int32 laplace_act_g_m( FMField *out, FMField *gc, FMField *mtx )
{
  int32 iqp, ic, ik, nEP, nQP, nCol;
  float64 val1, val2, val3;
  float64 *pout, *pmtx, *pg1, *pg2, *pg3;

  nEP = gc->nCol;
  nQP = gc->nLev;
  nCol = mtx->nCol;

/*   output( "%d %d %d %d\n", nEP, nQP, nCol, dim ); */

  switch (gc->nRow) {
  case 3:
    for (iqp = 0; iqp < nQP; iqp++) {
      pg1 = FMF_PtrLevel( gc, iqp );
      pg2 = pg1 + nEP;
      pg3 = pg2 + nEP;
      pout = FMF_PtrLevel( out, iqp );
      
      if (mtx->nLev == nQP) {
	pmtx = FMF_PtrLevel( mtx, iqp );
      } else {
	pmtx = FMF_PtrCurrent( mtx );
      }
      for (ic = 0; ic < nCol; ic++) {
	val1 = val2 = val3 = 0.0;
	for (ik = 0; ik < nEP; ik++) {
/* 	    output( "%d %d %d %d\n", iqp, ic, ik ); */
	  val1 += pg1[ik] * pmtx[ic+nCol*ik];
	  val2 += pg2[ik] * pmtx[ic+nCol*ik];
	  val3 += pg3[ik] * pmtx[ic+nCol*ik];
	}
	pout[ic+0] = val1;
	pout[ic+1] = val2;
	pout[ic+2] = val3;
      }
    }
    break;

  case 2:
    for (iqp = 0; iqp < nQP; iqp++) {
      pg1 = FMF_PtrLevel( gc, iqp );
      pg2 = pg1 + nEP;
      pout = FMF_PtrLevel( out, iqp );
      
      if (mtx->nLev == nQP) {
	pmtx = FMF_PtrLevel( mtx, iqp );
      } else {
	pmtx = FMF_PtrCurrent( mtx );
      }
      for (ic = 0; ic < nCol; ic++) {
	val1 = val2 = 0.0;
	for (ik = 0; ik < nEP; ik++) {
/* 	    output( "%d %d %d %d\n", iqp, ic, ik ); */
	  val1 += pg1[ik] * pmtx[ic+nCol*ik];
	  val2 += pg2[ik] * pmtx[ic+nCol*ik];
	}
	pout[ic+0] = val1;
	pout[ic+1] = val2;
      }
    }
    break;

  default:
    errput( ErrHead "ERR_Switch\n" );
    return( RET_Fail );
  }

  return( RET_OK );
}

#undef __FUNC__
#define __FUNC__ "laplace_act_gt_m"
/*!
  @par Revision history:
  - 28.11.2005, c
  - 30.05.2007
*/
int32 laplace_act_gt_m( FMField *out, FMField *gc, FMField *mtx )
{
  int32 iqp, iep, ii, nEP, nQP, nCol;
  float64 *pout, *pmtx, *pg1, *pg2, *pg3;

  nEP = gc->nCol;
  nQP = gc->nLev;
  nCol = mtx->nCol;

  switch (gc->nRow) {
  case 3:
    for (iqp = 0; iqp < nQP; iqp++) {
      pg1 = FMF_PtrLevel( gc, iqp );
      pg2 = pg1 + nEP;
      pg3 = pg2 + nEP;
      
      pmtx = FMF_PtrLevel( mtx, iqp );
      for (iep = 0; iep < nEP; iep++) {
	pout = FMF_PtrLevel( out, iqp ) + nCol * iep;
	for (ii = 0; ii < nCol; ii++) {
	  pout[ii]
	    = pg1[iep] * pmtx[0*nCol+ii]
	    + pg2[iep] * pmtx[1*nCol+ii]
	    + pg3[iep] * pmtx[2*nCol+ii];
	}
      }
    }
    break;

  case 2:
    for (iqp = 0; iqp < nQP; iqp++) {
      pg1 = FMF_PtrLevel( gc, iqp );
      pg2 = pg1 + nEP;
      
      pmtx = FMF_PtrLevel( mtx, iqp );
      for (iep = 0; iep < nEP; iep++) {
	pout = FMF_PtrLevel( out, iqp ) + nCol * iep;
	for (ii = 0; ii < nCol; ii++) {
	  pout[ii]
	    = pg1[iep] * pmtx[0*nCol+ii]
	    + pg2[iep] * pmtx[1*nCol+ii];
	}
      }
    }
    break;

  default:
    errput( ErrHead "ERR_Switch\n" );
    return( RET_Fail );
  }

  return( RET_OK );
}

#undef __FUNC__
#define __FUNC__ "dw_laplace"
/*!
  @par Revision history:
  - 28.11.2005, c
  - 09.12.2005
*/
int32 dw_laplace( FMField *out, FMField *grad,
		  FMField *coef, VolumeGeometry *vg,
		  int32 isDiff )
{
  int32 ii, dim, nQP, nEP, ret = RET_OK;
  FMField *gtg = 0, *gtgu = 0;

  nQP = vg->bfGM->nLev;
  nEP = vg->bfGM->nCol;
  dim = vg->bfGM->nRow;


  if (isDiff) {
    fmf_createAlloc( &gtg, 1, nQP, nEP, nEP );
  } else {
    fmf_createAlloc( &gtgu, 1, nQP, nEP, 1 );
  }

  for (ii = 0; ii < out->nCell; ii++) {
    FMF_SetCell( out, ii );
    FMF_SetCell( vg->bfGM, ii );
    FMF_SetCell( vg->det, ii );
    if (coef->nCell > 1)
      FMF_SetCell( coef, ii );

    if (isDiff) {
      laplace_build_gtg( gtg, vg->bfGM );
      fmf_mulAF( gtg, gtg, coef->val );
      fmf_sumLevelsMulF( out, gtg, vg->det->val );
    } else {
      FMF_SetCell( grad, ii );
      laplace_act_gt_m( gtgu, vg->bfGM, grad );
      fmf_mulAF( gtgu, gtgu, coef->val );
      fmf_sumLevelsMulF( out, gtgu, vg->det->val );
    }
    ERR_CheckGo( ret );
  }

 end_label:
  if (isDiff) {
    fmf_freeDestroy( &gtg );
  } else {
    fmf_freeDestroy( &gtgu );
  }

  return( ret );
}

#undef __FUNC__
#define __FUNC__ "d_laplace"
int32 d_laplace( FMField *out, FMField *gradP1, FMField *gradP2,
		 FMField *coef, VolumeGeometry *vg )
{
  int32 ii, dim, nQP, ret = RET_OK;
  FMField *dgp2 = 0, *gp1tdgp2 = 0;

  nQP = vg->bfGM->nLev;
  dim = vg->bfGM->nRow;

  fmf_createAlloc( &dgp2, 1, nQP, dim, 1 );
  fmf_createAlloc( &gp1tdgp2, 1, nQP, 1, 1 );

  for (ii = 0; ii < out->nCell; ii++) {
    FMF_SetCell( out, ii );
    FMF_SetCell( vg->det, ii );
    FMF_SetCell( gradP1, ii );
    FMF_SetCell( gradP2, ii );
    if (coef->nCell > 1)
      FMF_SetCell( coef, ii );

    fmf_mulAF( dgp2, gradP2, coef->val );
    fmf_mulATB_nn( gp1tdgp2, gradP1, dgp2 );
    fmf_sumLevelsMulF( out, gp1tdgp2, vg->det->val );

    ERR_CheckGo( ret );
  }

 end_label:
  fmf_freeDestroy( &dgp2 );
  fmf_freeDestroy( &gp1tdgp2 );

  return( ret );
}

#undef __FUNC__
#define __FUNC__ "dw_diffusion"
/*!
  @par Revision history:
  - c: 03.08.2006, r: 23.01.2008
*/
int32 dw_diffusion( FMField *out, FMField *grad,
		    FMField *mtxD, VolumeGeometry *vg,
		    int32 isDiff )
{
  int32 ii, dim, nQP, nEP, ret = RET_OK;
  FMField *gtd = 0, *gtdg = 0, *dgp = 0, *gtdgp = 0;

  nQP = vg->bfGM->nLev;
  nEP = vg->bfGM->nCol;
  dim = vg->bfGM->nRow;

  if (isDiff) {
    fmf_createAlloc( &gtd, 1, nQP, nEP, dim );
    fmf_createAlloc( &gtdg, 1, nQP, nEP, nEP );
  } else {
    fmf_createAlloc( &dgp, 1, nQP, dim, 1 );
    fmf_createAlloc( &gtdgp, 1, nQP, nEP, 1 );
  }

  for (ii = 0; ii < out->nCell; ii++) {
    FMF_SetCell( out, ii );
    FMF_SetCell( vg->bfGM, ii );
    FMF_SetCell( vg->det, ii );
    if (mtxD->nCell > 1) {
      FMF_SetCell( mtxD, ii );
    }

    if (isDiff) {
      fmf_mulATB_nn( gtd, vg->bfGM, mtxD );
      fmf_mulAB_nn( gtdg, gtd, vg->bfGM );
      fmf_sumLevelsMulF( out, gtdg, vg->det->val );
    } else {
      FMF_SetCell( grad, ii );
      fmf_mulAB_nn( dgp, mtxD, grad );
      fmf_mulATB_nn( gtdgp, vg->bfGM, dgp );
      fmf_sumLevelsMulF( out, gtdgp, vg->det->val );
    }
    ERR_CheckGo( ret );
  }

 end_label:
  if (isDiff) {
    fmf_freeDestroy( &gtd );
    fmf_freeDestroy( &gtdg );
  } else {
    fmf_freeDestroy( &dgp );
    fmf_freeDestroy( &gtdgp );
  }

  return( ret );
}


#undef __FUNC__
#define __FUNC__ "d_diffusion"
/*!
  @par Revision history:
  - c: 12.03.2007, r: 23.01.2008
*/
int32 d_diffusion( FMField *out, FMField *gradP1, FMField *gradP2,
		   FMField *mtxD, VolumeGeometry *vg )
{
  int32 ii, dim, nQP, ret = RET_OK;
  FMField *dgp2 = 0, *gp1tdgp2 = 0;

  nQP = vg->bfGM->nLev;
  dim = vg->bfGM->nRow;

  fmf_createAlloc( &dgp2, 1, nQP, dim, 1 );
  fmf_createAlloc( &gp1tdgp2, 1, nQP, 1, 1 );

  for (ii = 0; ii < out->nCell; ii++) {
    FMF_SetCell( out, ii );
    FMF_SetCell( vg->det, ii );
    FMF_SetCell( gradP1, ii );
    FMF_SetCell( gradP2, ii );
    if (mtxD->nCell > 1) {
      FMF_SetCell( mtxD, ii );
    }

    fmf_mulAB_nn( dgp2, mtxD, gradP2 );
    fmf_mulATB_nn( gp1tdgp2, gradP1, dgp2 );
    fmf_sumLevelsMulF( out, gp1tdgp2, vg->det->val );

    ERR_CheckGo( ret );
  }

 end_label:
  fmf_freeDestroy( &dgp2 );
  fmf_freeDestroy( &gp1tdgp2 );

  return( ret );
}

#undef __FUNC__
#define __FUNC__ "dw_permeability_r"
/*!
  @par Revision history:
  - c: 23.04.2007, r: 23.01.2008
*/
int32 dw_permeability_r( FMField *out, FMField *mtxD, VolumeGeometry *vg )
{
  int32 ii, nQP, nEP, ret = RET_OK;
  FMField *gtd = 0;

  nQP = vg->bfGM->nLev;
  nEP = vg->bfGM->nCol;

  fmf_createAlloc( &gtd, 1, nQP, nEP, 1 );

  for (ii = 0; ii < out->nCell; ii++) {
    FMF_SetCell( out, ii );
    FMF_SetCell( vg->bfGM, ii );
    FMF_SetCell( vg->det, ii );
    if (mtxD->nCell > 1) {
      FMF_SetCell( mtxD, ii );
    }

    fmf_mulATB_nn( gtd, vg->bfGM, mtxD );
    fmf_sumLevelsMulF( out, gtd, vg->det->val );
    ERR_CheckGo( ret );
  }

 end_label:
  fmf_freeDestroy( &gtd );

  return( ret );
}

#undef __FUNC__
#define __FUNC__ "dw_diffusion_coupling"
int32 dw_diffusion_coupling( FMField *out, FMField *state, int32 offset,
			     FMField *mtxD, FMField *bf, VolumeGeometry *vg,
			     int32 *conn, int32 nEl, int32 nEP,
			     int32 *elList, int32 elList_nRow,
			     int32 isDiff, int32 mode)
{
  int32 ii, iel, dim, nQP, ret = RET_OK;
  FMField *st = 0, *gtd = 0, *gtdg = 0, *gp = 0, *dgp = 0, *gtdgp = 0;

  nQP = vg->bfGM->nLev;
  dim = vg->bfGM->nRow;

/*   output( "%d %d %d %d %d %d\n", offset, nEl, nEP, nQP, dim, elList_nRow ); */
  state->val = FMF_PtrFirst( state ) + offset;

  if (isDiff) {
    fmf_createAlloc( &gtd, 1, nQP, nEP, 1 );
    fmf_createAlloc( &gtdg, 1, nQP, nEP, nEP );
  } else {
    if (mode > 0) {
      fmf_createAlloc( &gp, 1, nQP, dim, 1 );
      fmf_createAlloc( &dgp, 1, nQP, 1, 1 );
    }
    else {
      fmf_createAlloc( &gp, 1, nQP, 1, 1 );
      fmf_createAlloc( &dgp, 1, nQP, dim, 1 );
    }
    fmf_createAlloc( &st, 1, 1, nEP, 1 );

    fmf_createAlloc( &gtdgp, 1, nQP, nEP, 1 );
  }

  for (ii = 0; ii < elList_nRow; ii++) {
    iel = elList[ii];

    FMF_SetCell( out, ii );
    FMF_SetCell( vg->bfGM, iel );
    FMF_SetCell( vg->det, iel );
    if (mtxD->nCell > 1) {
      FMF_SetCell( mtxD, ii );
    }

    if (isDiff) {
      fmf_mulATB_nn( gtd, vg->bfGM, mtxD );
      if (mode > 0)
	fmf_mulATBT_nn( gtdg, bf, gtd );
      else
	fmf_mulAB_nn( gtdg, gtd, bf );
      fmf_sumLevelsMulF( out, gtdg, vg->det->val );
    } else {
      ele_extractNodalValuesNBN( st, state, conn + nEP * iel );
      if (mode > 0) {
	fmf_mulAB_n1( gp, vg->bfGM, st );
	fmf_mulATB_nn( dgp, mtxD, gp );
	fmf_mulATB_nn( gtdgp, bf, dgp );
      }
      else {
	fmf_mulAB_n1( gp, bf, st );
	fmf_mulAB_nn( dgp, mtxD, gp );
	fmf_mulATB_nn( gtdgp, vg->bfGM, dgp );
      }
      fmf_sumLevelsMulF( out, gtdgp, vg->det->val );
    }
    ERR_CheckGo( ret );
  }

 end_label:
  if (isDiff) {
    fmf_freeDestroy( &gtd );
    fmf_freeDestroy( &gtdg );
  } else {
    fmf_freeDestroy( &st );
    fmf_freeDestroy( &gp );
    fmf_freeDestroy( &dgp );
    fmf_freeDestroy( &gtdgp );
  }

  return( ret );
}

#undef __FUNC__
#define __FUNC__ "d_diffusion_coupling"
int32 d_diffusion_coupling( FMField *out, FMField *stateP, FMField *stateQ,
			    FMField *mtxD, FMField *bf, VolumeGeometry *vg,
			    int32 *conn, int32 nEl, int32 nEP,
			    int32 *elList, int32 elList_nRow,
			    int32 isDiff, int32 mode)
{
  int32 ii, iel, dim, nQP, ret = RET_OK;
  FMField *st = 0, *aux1 = 0, *aux2 = 0, *aux3 = 0, *aux4 = 0;

  nQP = vg->bfGM->nLev;
  dim = vg->bfGM->nRow;

  fmf_createAlloc( &aux1, 1, nQP, dim, 1 );
  fmf_createAlloc( &aux2, 1, nQP, 1, 1 );
  fmf_createAlloc( &aux3, 1, nQP, nEP, 1 );
  fmf_createAlloc( &aux4, 1, nQP, 1, 1 );
  fmf_createAlloc( &st, 1, 1, nEP, 1 );


  for (ii = 0; ii < elList_nRow; ii++) {
    iel = elList[ii];

    FMF_SetCell( out, ii );
    FMF_SetCell( vg->bfGM, iel );
    FMF_SetCell( vg->det, iel );
    if (mtxD->nCell > 1) {
      FMF_SetCell( mtxD, ii );
    }

    ele_extractNodalValuesNBN( st, stateP, conn + nEP * iel );
    if (mode > 0) {
      fmf_mulAB_n1( aux2, vg->bfGM, st );
      fmf_mulATB_nn( aux1, mtxD, aux2 );
      fmf_mulATB_nn( aux3, bf, aux1 );
    }
    else {
      fmf_mulAB_n1( aux1, bf, st );
      fmf_mulAB_nn( aux2, mtxD, aux1 );
      fmf_mulATB_nn( aux3, vg->bfGM, aux2 );
    }
    ele_extractNodalValuesNBN( st, stateQ, conn + nEP * iel );
    fmf_mulATB_1n( aux4, st, aux3 );

    fmf_sumLevelsMulF( out, aux4, vg->det->val );

    ERR_CheckGo( ret );
  }

 end_label:
  fmf_freeDestroy( &st );
  fmf_freeDestroy( &aux1 );
  fmf_freeDestroy( &aux2 );
  fmf_freeDestroy( &aux3 );
  fmf_freeDestroy( &aux4 );

  return( ret );
}


#undef __FUNC__
#define __FUNC__ "d_surface_flux"
int32 d_surface_flux( FMField *out, FMField *grad,
                      FMField *mtxD, SurfaceGeometry *sg, int32 mode )
{
  int32 ii, dim, nQP, ret = RET_OK;
  FMField *dgp = 0, *ntdgp = 0;

  nQP = sg->normal->nLev;
  dim = sg->normal->nRow;

  fmf_createAlloc( &dgp, 1, nQP, dim, 1 );
  fmf_createAlloc( &ntdgp, 1, nQP, 1, 1 );

  for (ii = 0; ii < out->nCell; ii++) {
    FMF_SetCell( out, ii );
    FMF_SetCell( grad, ii );
    FMF_SetCell( mtxD, ii );
    FMF_SetCell( sg->normal, ii );
    FMF_SetCell( sg->det, ii );

    fmf_mulAB_nn( dgp, mtxD, grad );
    fmf_mulATB_nn( ntdgp, sg->normal, dgp );

    fmf_sumLevelsMulF( out, ntdgp, sg->det->val );
    if (mode == 1) {
      FMF_SetCell( sg->area, ii );
      fmf_mulC( out, 1.0 / sg->area->val[0] );
    }
    ERR_CheckGo( ret );
  }

 end_label:
  fmf_freeDestroy( &dgp );
  fmf_freeDestroy( &ntdgp );

  return( ret );
}
