def read_elastica_data():
    import h5py

    # Read from the metadata file
    fn = "elastica_metadata.h5"

    # The convention that we follow is
    # Group keys are either numbers or begin with Capital letters
    # Dataset keys begin with small letters
    with h5py.File(fn, "r") as f:
        # Metadata is indexed by unique keys
        access = f["000050"]

        # Time information
        time_data = access["TimeMetadata"]
        # Scalar datasets, so do [()]
        print("dt: ", time_data["dt"][()])
        print("time: ", time_data["time"][()])
        step_information = time_data["Timestep"]
        print("step: ", step_information["step"][()])
        print("substep: ", step_information["substep"][()])
        print("threshold: ", step_information["threshold"][()])

        # Alternatively you can also do
        print("time: ", access["TimeMetadata"]["time"][()])

        # System information
        system_data = access["data"]
        cosserat_rod_data = system_data["CosseratRod"]
        for idx in range(4):
            slice = cosserat_rod_data[str(idx)]
            # you can use tags
            position_group = slice["Position"]
            print("position shape: ", position_group["data"].shape)

        # Alternatively you can do
        print(
            "position shape: ",
            access
            # fmt: off
            ["data"]  # physical data
            ["CosseratRod"]  # system type
            ["0"]  # index as string
            ["Position"]  # member
            ["data"]
            # physical data of member
            # fmt: on
            .shape,
        )

    # Alternatively you can directly access the data file bypassing the metadata file
    with h5py.File("elastica_000050.h5", "r") as f:
        print(
            "position shape: ",
            f
            # fmt: off
            ["CosseratRod"]  # system type
            ["0"]  # index as string
            ["Position"]  # member
            ["data"]
            # physical data of member
            # fmt: on
            .shape,
        )


if __name__ == "__main__":
    read_elastica_data()
