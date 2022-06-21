# Distributed under the MIT License.
# See LICENSE.txt for details.

ELASTICA_VERSION = "@elastica_VERSION@"


def main():
    import argparse

    class HelpFormatter(argparse.ArgumentDefaultsHelpFormatter,
                        argparse.RawDescriptionHelpFormatter):
        pass

    parser = argparse.ArgumentParser(
        prog='elastica',
        description="elastica version: {}".format(ELASTICA_VERSION),
        formatter_class=HelpFormatter)

    # Version endpoint
    parser.add_argument('--version', action='version', version=ELASTICA_VERSION)

    parser.parse_args()
    raise NotImplementedError("No subprograms are implemented yet.")


if __name__ == '__main__':
    main()
