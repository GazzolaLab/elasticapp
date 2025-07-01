import numpy as np
import h5py

from mpi4py.MPI import Intracomm, MAX

from elasticapp.Systems.FishModel._PyFishModel import Fish


class MPIFishWriter:
    def __init__(
        self, 
        mpi_comm: Intracomm,
        fish_list: list[Fish],
        index_list: list[int],
        num_fish_element: int,
        max_num_fish: int,
    ) -> None:
        self.num_fish = len(fish_list)
        if self.num_fish != len(index_list):
            msg = "index_list must have the same length as fish_list."
            raise ValueError(msg)

        self.mpi_comm = mpi_comm
        self.max_num_fish = max_num_fish
        
        self.fish_dicts = []
        dummy_node = np.zeros((3, num_fish_element + 1), dtype=np.float64)
        dummy_elem = np.zeros((3, num_fish_element), dtype=np.float64)
        dummy_director = np.zeros((3, 3, num_fish_element), dtype=np.float64)
        for i in range(max_num_fish):
            if i < self.num_fish:
                self.fish_dicts.append(
                    {
                        "name": f"fish_{index_list[i]}",
                        "position": np.asarray(fish_list[i].position_collection),
                        "director": np.asarray(fish_list[i].director_collection),
                        "velocity": np.asarray(fish_list[i].velocity_collection),
                        "angular_velocity": np.asarray(fish_list[i].omega_collection),
                    }
                )
            else:
                self.fish_dicts.append(
                    {
                        "name": f"dummy_{mpi_comm.Get_rank()}",
                        "position": dummy_node,
                        "director": dummy_director,
                        "velocity": dummy_node,
                        "angular_velocity": dummy_elem,
                    }
                )
        
            
    def write_to_file(self, filename: str) -> None:
        with h5py.File(filename, "w", driver="mpio", comm=self.mpi_comm) as f:
            root = f.create_group("root", track_order=True)
            
            for fish_dict in self.fish_dicts:
                subgroup = root.create_group(fish_dict["name"])
                
                for key, value in fish_dict.items():
                    if key != "name":
                        subgroup.create_dataset(
                            name=key, 
                            data=value, 
                            dtype=value.dtype, 
                        )
                

if __name__ == "__main__":
    from mpi4py import MPI
    from elasticapp.Systems import FishModel
    from elasticapp.CaseStudies.PyFishCaseStudy import FishCaseStudy
    from elasticapp.Arrays._PyArrays import Matrix
    
    mpi_rank = MPI.COMM_WORLD.Get_rank()
    mpi_size = MPI.COMM_WORLD.Get_size()
    
    filename = "fish_data.h5"
    if mpi_rank == 0:
        import os
        if os.path.exists(filename):
            os.remove(filename)
    
    origins = np.loadtxt("fish_head_positions.txt", delimiter=",")
    num_fish_total = origins.shape[1]
    num_fish_per_rank = np.ones((mpi_size,), np.int32) * (num_fish_total // mpi_size)
    rem = num_fish_total % mpi_size
    num_fish_per_rank[:rem] += 1
    index = np.insert(np.cumsum(num_fish_per_rank), 0, 0)
    if mpi_size == 1:
        origin = Matrix(origins)
    else:
        tmp = origins[:, index[mpi_rank] : index[mpi_rank + 1]].copy()
        origin = Matrix(tmp)

    has_fish = num_fish_per_rank[mpi_rank] > 0
    if has_fish:
        cs = FishCaseStudy(
            num_fish=int(num_fish_per_rank[mpi_rank]),
            origins=origin,
            n_elements=60,
            density=1000.0,
            length=1.0,
            damping_rate=0.16,
            youngs_modulus=1.0e4,
            dt_scale=5e-4,
            period=1.0,
        )
        elastic_fish_list, _, _, _ = cs.communicate()
    else:
        elastic_fish_list = []
        
    writer = MPIFishWriter(
        mpi_comm=MPI.COMM_WORLD,
        fish_list=elastic_fish_list,
        index_list=list(range(index[mpi_rank], index[mpi_rank + 1])),
        num_fish_element=60,
        max_num_fish=np.amax(num_fish_per_rank)
    )
    writer.write_to_file(filename)
    
    