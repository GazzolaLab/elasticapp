# Distributed under the MIT License.
# See LICENSE.txt for details.

import elasticapp

def main():
    import argparse

    class HelpFormatter(argparse.ArgumentDefaultsHelpFormatter,
                        argparse.RawDescriptionHelpFormatter):
        pass

    parser = argparse.ArgumentParser(
        prog='elastica',
        description="elastica version: {}".format(elasticapp.__version__),
        formatter_class=HelpFormatter)

    # Version endpoint
    parser.add_argument('--version', action='version', version=elasticapp.__version__)

    parser.parse_args()
    raise NotImplementedError("No subprograms are implemented yet.")


if __name__ == '__main__':
    main()
