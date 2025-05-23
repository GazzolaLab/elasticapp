from warnings import warn

import numpy as np
import h5py

from mpi4py.MPI import Intracomm


class VTKHDFImageDataWriter:
    def __init__(
        self,
        mpi_comm: Intracomm,
        dx: float,
        global_grid_size: tuple[int, int, int],
        local_grid_size: tuple[int, int, int],
        ghost_size: int,
    ) -> None:
        self.mpi_comm = mpi_comm
        self.global_grid_size = global_grid_size
        self.origin = (0.5 * dx,) * 3
        self.spacing = (dx,) * 3

        start_idx = np.asarray(self.mpi_comm.coords) * local_grid_size
        end_idx = start_idx + local_grid_size
        self.slice = tuple(slice(start_idx[i], end_idx[i]) for i in range(3)) + (
            slice(None),
        )
        self.no_ghost_slice = (slice(None),) + (slice(ghost_size, -ghost_size),) * 3
        self.local_size_with_ghost = tuple(
            local_grid_size[i] + 2 * ghost_size for i in range(3)
        )

    def write_to_file(self, filename: str, **kwargs) -> None:
        with h5py.File(filename, "w", driver="mpio", comm=self.mpi_comm) as f:
            image_data_group = f.create_group(name="VTKHDF")
            image_data_group.attrs["Version"] = (2, 2)
            image_data_group.attrs["Type"] = np.string_("ImageData")
            image_data_group.attrs["WholeExtent"] = (
                0,
                self.global_grid_size[2] - 1,
                0,
                self.global_grid_size[1] - 1,
                0,
                self.global_grid_size[0] - 1,
            )
            image_data_group.attrs["Direction"] = (
                1.0,
                0.0,
                0.0,
                0.0,
                1.0,
                0.0,
                0.0,
                0.0,
                1.0,
            )
            image_data_group.attrs["Origin"] = self.origin
            image_data_group.attrs["Spacing"] = self.spacing

            point_data_group = image_data_group.create_group(name="PointData")
            # cell_data_group = image_data_group.create_group(name="CellData")
            # field_data_group = image_data_group.create_group(name="FieldData")

            for key, value in kwargs.items():
                if not isinstance(value, np.ndarray):
                    warn(f"Value for {key} is not a numpy array. Skipped.")
                    continue

                if len(value.shape) != 3 and len(value.shape) != 4:
                    warn(f"Value for {key} is not a 3D or 4D array. Skipped.")
                    continue

                if len(value.shape) == 3:
                    if value.shape != self.local_size_with_ghost:
                        warn(
                            f"Value for {key} has shape {value.shape} "
                            f"but expected {self.local_size_with_ghost}. Skipped."
                        )
                        continue

                    data_to_fill = point_data_group.create_dataset(
                        name=key,
                        shape=self.global_grid_size,
                        dtype=np.float64,
                    )
                    data_to_fill[self.slice[:-1]] = value[
                        self.no_ghost_slice[1:]
                    ].copy()

                if len(value.shape) == 4:
                    if value.shape != (3,) + self.local_size_with_ghost:
                        warn(
                            f"Value for {key} has shape {value.shape} "
                            f"but expected {(3,) + self.local_size_with_ghost}. Skipped."
                        )
                        continue

                    data_to_fill = point_data_group.create_dataset(
                        name=key,
                        shape=(*self.global_grid_size, 3),
                        dtype=np.float64,
                    )
                    data_to_fill[self.slice] = np.transpose(
                        value[self.no_ghost_slice], (1, 2, 3, 0)
                    ).copy()


if __name__ == "__main__":
    # Example usage of VTKHDFImageDataWriter
    grid_size = (32, 64, 128)  # z-y-x
    from sopht_mpi.simulator import UnboundedFlowSimulator3D

    sim = UnboundedFlowSimulator3D(
        grid_size=grid_size,
        x_range=1.0,
        kinematic_viscosity=0.1,
        flow_type="navier_stokes_with_forcing",
        real_t=np.float64,
        rank_distribution=(1, 2, 2),
        filter_vorticity=False,
        ghost_size=2,
    )
    sim.velocity_field[0, :, :, :] = (
        np.sin(np.pi * sim.position_field[0, :, :, :])
        * np.sin(2 * np.pi * sim.position_field[1, :, :, :])
        * np.sin(3 * np.pi * sim.position_field[2, :, :, :])
    )

    writer = VTKHDFImageDataWriter(
        mpi_comm=sim.mpi_construct.grid,
        dx=sim.dx,
        global_grid_size=sim.grid_size,
        local_grid_size=sim.mpi_construct.local_grid_size,
        ghost_size=sim.ghost_size,
    )
    writer.write_to_file("test_flow_io.vtkhdf", velocity=sim.velocity_field)
