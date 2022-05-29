import numpy as np


def analytical_shearable(
    youngs_modulus: float,
    shear_modulus: float,
    applied_force: float,
    radius: float,
    length: float,
    n_elem: int = 500,
):
    arg_s = np.linspace(0.0, length, n_elem)
    if type(applied_force) is np.ndarray:
        acting_force = applied_force[np.nonzero(applied_force)]
    else:
        acting_force = applied_force
    acting_force = np.abs(acting_force)

    cs_area = np.pi * radius ** 2
    shear_shape_factor = 13.5 / 14.0
    shear = shear_shape_factor * shear_modulus * cs_area

    linear_prefactor = -acting_force / shear
    # ?
    I = cs_area**2 / (4.0 * np.pi)
    bend = youngs_modulus * I
    quadratic_prefactor = -acting_force * length / 2.0 / bend
    cubic_prefactor = acting_force / 6.0 / bend
    return arg_s, arg_s * (
        linear_prefactor + arg_s * (quadratic_prefactor + arg_s * cubic_prefactor)
    )


def analytical_unshearable(
    youngs_modulus: float,
    shear_modulus: float,
    applied_force: float,
    radius: float,
    length: float,
    n_elem: int = 500,
):
    arg_s = np.linspace(0.0, length, n_elem)
    if type(applied_force) is np.ndarray:
        acting_force = applied_force[np.nonzero(applied_force)]
    else:
        acting_force = applied_force
    acting_force = np.abs(acting_force)

    cs_area = np.pi * radius**2
    I = cs_area**2 / (4.0 * np.pi)
    bend = youngs_modulus * I
    quadratic_prefactor = -acting_force * length / 2.0 / bend
    cubic_prefactor = acting_force / 6.0 / bend
    return arg_s, arg_s**2 * (quadratic_prefactor + arg_s * cubic_prefactor)
