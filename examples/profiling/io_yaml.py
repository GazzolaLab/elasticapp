def read_elastica_profile():
    import yaml

    fn = "filename.yml"
    with open(fn, 'r') as f:
        data = yaml.load(f, Loader=yaml.Loader)

    # NOTE : timers node is from our main file
    apply_over_systems_info = data["timers"]["sections"]["ApplyOverSystems"]
    assert len(apply_over_systems_info) > 0
    assert type(apply_over_systems_info["total"]) == float
    assert type(apply_over_systems_info["average"]) == float


if __name__ == "__main__":
    read_elastica_profile()
