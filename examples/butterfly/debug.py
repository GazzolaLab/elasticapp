from elasticapp.Systems import CosseratRods  # noqa
import numpy as np


def as_dict(rod):
    all_tags = [
        rod.Acceleration,
        rod.AngularAcceleration,
        rod.AngularVelocity,
        rod.BendingTwistRigidityMatrix,
        rod.Curvature,
        rod.Director,
        rod.ElementDilatation,
        rod.ElementDimension,
        rod.ElementLength,
        rod.ElementVolume,
        rod.ExternalLoads,
        rod.ExternalTorques,
        # rod.ForceDampingRate,
        rod.InternalCouple,
        rod.InternalLoads,
        rod.InternalStress,
        rod.InternalTorques,
        rod.InvMassSecondMomentOfInertia,
        rod.Mass,
        rod.MassSecondMomentOfInertia,
        rod.Material,
        rod.NElement,
        rod.Position,
        rod.ReferenceCurvature,
        rod.ReferenceElementLength,
        rod.ReferenceShearStretchStrain,
        rod.ReferenceVoronoiLength,
        rod.ShearStretchRigidityMatrix,
        rod.ShearStretchStrain,
        rod.Tangent,
        # rod.TorqueDampingRate,
        rod.Velocity,
        rod.VoronoiDilatation,
        rod.VoronoiLength,
    ]
    keys = map(lambda x: x.__class__.__name__.lstrip("_"), all_tags)
    vals = map(lambda x: np.array(rod.loc[x]), all_tags)
    return dict(zip(keys, vals))
