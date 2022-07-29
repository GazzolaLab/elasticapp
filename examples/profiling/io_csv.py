import pandas as pd


def read_elastica_profile():
    data = pd.read_csv("filename.csv", sep=",", comment="#")
    # can be converted to numpy, hdf, xml etc.
    assert len(data.columns) > 0


if __name__ == "__main__":
    read_elastica_profile()
