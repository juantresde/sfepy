import numpy as nm

from sfepy.base.base import use_method_with_name
from sfepy.terms.terms import Term, terms
from sfepy.terms.terms_base import ScalarScalar

class DiffusionTerm(Term):
    r"""
    :Description:
    General diffusion term with permeability :math:`K_{ij}`. Can be
    evaluated. Can use derivatives.

    :Definition:
    .. math::
        \int_{\Omega} K_{ij} \nabla_i q \nabla_j p \mbox{ , } \int_{\Omega}
        K_{ij} \nabla_i \bar{p} \nabla_j r

    :Arguments 1:
        material : :math:`K_{ij}`,
        virtual  : :math:`q`,
        state    : :math:`p`

    :Arguments 2:
        material    : :math:`K_{ij}`,
        parameter_1 : :math:`\bar{p}`,
        parameter_2 : :math:`r`
    """
    name = 'dw_diffusion'
    arg_types = (('material', 'virtual', 'state'),
                 ('material', 'parameter_1', 'parameter_2'))
    modes = ('weak', 'eval')
    symbolic = {'expression': 'div( K * grad( u ) )',
                'map' : {'u' : 'state', 'K' : 'material'}}

    def get_fargs(self, mat, virtual, state,
                  mode=None, term_mode=None, diff_var=None, **kwargs):
        vg, _ = self.get_mapping(state)

        if mat is None:
            if self.name == 'dw_laplace':
                n_el, n_qp, _, _, _ = self.get_data_shape(state)
                mat = nm.ones((1, n_qp, 1, 1), dtype=nm.float64)

        if mode == 'weak':
            if diff_var is None:
                grad = self.get(state, 'grad')
                fmode = 0

            else:
                grad = nm.array([0], ndmin=4, dtype=nm.float64)
                fmode = 1

            return grad, mat, vg, fmode

        elif mode == 'eval':
            grad1 = self.get(virtual, 'grad')
            grad2 = self.get(state, 'grad')

            return grad1, grad2, mat, vg

        else:
            raise ValueError('unsupported evaluation mode in %s! (%s)'
                             % (self.name, mode))

    def get_eval_shape(self, mat, virtual, state,
                       mode=None, term_mode=None, diff_var=None, **kwargs):
        n_el, n_qp, dim, n_en, n_c = self.get_data_shape(state)

        return (n_el, 1, 1, 1), state.dtype

    def set_arg_types(self):
        if self.mode == 'weak':
            self.function = terms.dw_diffusion

        else:
            self.function = terms.d_diffusion

class LaplaceTerm(DiffusionTerm):
    r"""
    :Description:
    Laplace term with :math:`c` coefficient. Can be
    evaluated. Can use derivatives.

    :Definition:
    .. math::
        \int_{\Omega} c \nabla q \cdot \nabla p \mbox{ , } \int_{\Omega}
        c \nabla \bar{p} \cdot \nabla r

    :Arguments 1:
        material : :math:`c`,
        virtual  : :math:`q`,
        state    : :math:`p`

    :Arguments 2:
        material    : :math:`c`,
        parameter_1 : :math:`\bar{p}`,
        parameter_2 : :math:`r`
    """
    name = 'dw_laplace'
    arg_types = (('opt_material', 'virtual', 'state'),
                 ('opt_material', 'parameter_1', 'parameter_2'))
    modes = ('weak', 'eval')
    symbolic = {'expression': 'c * div( grad( u ) )',
                'map' : {'u' : 'state', 'c' : 'material'}}

    def set_arg_types(self):
        if self.mode == 'weak':
            self.function = terms.dw_laplace

        else:
            self.function = terms.d_laplace

# class PermeabilityRTerm( Term ):
#     r"""
#     :Description:
#     Special-purpose diffusion-like term with permeability :math:`K_{ij}` (to
#     use on the right-hand side).

#     :Definition:
#     .. math::
#         \int_{\Omega} K_{ij} \nabla_j q

#     :Arguments:
#         material : :math:`K_{ij}`,
#         virtual  : :math:`q`,
#         index    : :math:`i`
#     """
#     name = 'dw_permeability_r'
#     arg_types = ('material', 'virtual', 'index')

#     function = staticmethod(terms.dw_permeability_r)
        
#     def __call__( self, diff_var = None, chunk_size = None, **kwargs ):
#         mat, virtual, index = self.get_args( **kwargs )
#         ap, vg = self.get_approximation(virtual)
#         n_el, n_qp, dim, n_ep = ap.get_v_data_shape(self.integral)

#         if diff_var is None:
#             shape = (chunk_size, 1, n_ep, 1)
#         else:
#             raise StopIteration

#         if isinstance(index, list):
#             index = index[0]

#         mat = nm.ascontiguousarray(mat[...,index:index+1])
#         for out, chunk in self.char_fun( chunk_size, shape ):
#             status = self.function( out, mat, vg, ap.econn, chunk )
#             yield out, chunk, status

class DiffusionRTerm(Term):
    r"""
    :Description:
    Diffusion-like term with material parameter :math:`K_{j}` (to
    use on the right-hand side).

    :Definition:
    .. math::
        \int_{\Omega} K_{j} \nabla_j q

    :Arguments:
        material : :math:`K_j`,
        virtual  : :math:`q`
    """
    name = 'dw_diffusion_r'
    arg_types = ('material', 'virtual')
    function = staticmethod(terms.dw_permeability_r)

    def get_fargs(self, mat, virtual,
                  mode=None, term_mode=None, diff_var=None, **kwargs):

        vg, _ = self.get_mapping(virtual)
        return mat, vg

class DiffusionCoupling(ScalarScalar, Term):
    r"""
    :Description:
    Diffusion copupling term with material parameter :math:`K_{j}`.

    :Definition:
    .. math::
        \int_{\Omega}  p K_{j} \nabla_j q

    :Arguments:
        material : :math:`K_{j}`,
        virtual  : :math:`q`,
        state    : :math:`p`
    """
    name = 'dw_diffusion_coupling'
    arg_types = (('material', 'virtual', 'state'),
                 ('material', 'state', 'virtual'),
                 ('material', 'parameter_1', 'parameter_2'))
    modes = ('weak0', 'weak1', 'eval')

    @staticmethod
    def d_eval(out, mat, val, grad, vg):
        out_qp = grad * mat * val

        status = vg.integrate(out, out_qp)

        return status

    def get_fargs( self, mat, virtual, state,
                   mode=None, term_mode=None, diff_var=None, **kwargs):
        ap, vg = self.get_approximation(virtual)

        if mode == 'weak':
            term_mode = int(self.mode[-1])

            aps, vgs = self.get_approximation(state)
            bf = aps.get_base('v', 0, self.integral)

            if diff_var is None:
                if term_mode > 0:
                    val = self.get(virtual, 'grad')
                else:
                    val = self.get(state, 'val')

                fmode = 0

            else:
                val = nm.array([0], ndmin=4, dtype=nm.float64)
                fmode = 1

            return val, mat, bf, vg, term_mode, fmode

        elif mode == 'eval':

            grad = self.get(virtual, 'grad')
            val = self.get(state, 'val')

            return mat, val, grad, vg

        else:
            raise ValueError('unsupported evaluation mode in %s! (%s)'
                             % (self.name, mode))

    def get_eval_shape(self, mat, virtual, state,
                       mode=None, term_mode=None, diff_var=None, **kwargs):
        n_el, n_qp, dim, n_en, n_c = self.get_data_shape(state)

        return (n_el, 1, 1, 1), state.dtype

    def set_arg_types( self ):
        if self.mode[:-1] == 'weak':
            self.function = terms.dw_diffusion_coupling
        else:
            self.function = self.d_eval

class DiffusionVelocityTerm( Term ):
    r"""
    :Description:
    Diffusion velocity averaged in elements.

    :Definition:
    .. math::
        \mbox{vector for } K \from \Ical_h: \int_{T_K} -K_{ij} \nabla_j \bar{p}
        / \int_{T_K} 1

    :Arguments:
        material  : :math:`K_{ij}`,
        parameter : :math:`\bar{p}`
    """
    name = 'de_diffusion_velocity'
    arg_types = ('material', 'parameter')
    default_mode = (1, True)

    @staticmethod
    def function(out, grad, mat, vg, mode):

        aux = nm.sum(mat * grad, axis=3)[:,:,:,nm.newaxis]
        status = vg.integrate(out, nm.ascontiguousarray(aux), mode)

        return status

    def get_fargs(self, mat, parameter,
                  mode=None, term_mode=None, diff_var=None, **kwargs):
        vg, _ = self.get_mapping(parameter)
        grad = self.get(parameter, 'grad')
        fmode = {'eval' : 0, 'el_avg' : 1}.get(mode, self.default_mode[0])
        if self.default_mode[1]:
            mat *= -1.0

        return grad, mat, vg, fmode

    def get_eval_shape(self, mat, parameter,
                       mode=None, term_mode=None, diff_var=None, **kwargs):
        n_el, n_qp, dim, n_en, n_c = self.get_data_shape(parameter)

        if mode != 'qp':
            n_qp = 1

        return (n_el, n_qp, dim, 1), parameter.dtype

class DiffusionIntegrateTerm(DiffusionVelocityTerm):
    r"""
    :Description:
    Diffusion integrate term.

    :Definition:
    .. math::
        \int_{\Omega} K_{ij} \nabla_j \bar{p}

    :Arguments:
        material: :math:`K_{ij}`,
        parameter:  :math:`\bar{p}`,
    """
    name = 'di_diffusion_integrate'
    arg_types = ('material', 'parameter')
    default_mode = (0, False)

class SurfaceFluxTerm(Term):
    r"""
    :Description:
    Surface flux term.

    :Definition:
    .. math::
        \int_{\Gamma} \ul{n} \cdot K_{ij} \nabla_j \bar{p}

    :Arguments:
        material: :math:`\ul{K}`,
        parameter:  :math:`\bar{p}`,
    """
    name = 'd_surface_flux'
    arg_types = ('material', 'parameter')
    integration = 'surface_extra'

    function = staticmethod(terms.d_surface_flux)

    def get_fargs(self, mat, parameter,
                  mode=None, term_mode=None, diff_var=None, **kwargs):
        sg, _ = self.get_mapping(parameter)

        grad = self.get(parameter, 'grad')

        fmode = {'eval' : 0, 'el_avg' : 1}.get(mode, 1)

        return grad, mat, sg, fmode

    def get_eval_shape(self, mat, parameter,
                       mode=None, term_mode=None, diff_var=None, **kwargs):
        n_fa, n_qp, dim, n_en, n_c = self.get_data_shape(parameter)

        return (n_fa, 1, 1, 1), parameter.dtype
