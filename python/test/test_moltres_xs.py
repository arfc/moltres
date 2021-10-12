import json
import os


def test_serpent_xs():
    """Testing moltres_xs.py with Serpent 2 XS data, compared
    with the expected JSON file in the gold directory
    """

    # Run moltres_xs with Serpent 2 data
    os.system('python ../moltres_xs.py msfr_xs.inp')

    # Load the output json xs file
    with open('msfrXS.json') as f:
        data = json.load(f)

    # Load gold copy of json xs file
    with open('gold/msfrXS.json') as h:
        expected = json.load(h)

    # Check if they match
    assert data == expected

    # Delete output json xs file after check
    if os.path.exists('msfrXS.json'):
        os.remove('msfrXS.json')
    else:
        print('Attempted to delete JSON output file but it does not exist')


def test_scale_xs():
    """Testing moltres_xs.py with SCALE XS data, compared
    with the expected JSON file in the gold directory
    """

    # Run moltres_xs with SCALE data
    os.system('python ../moltres_xs.py pin_cell_XS.inp')

    # Load the output json xs file
    with open('PinXS.json') as f:
        data = json.load(f)

    # Load gold copy of json xs file
    with open('gold/PinXS.json') as h:
        expected = json.load(h)

    # Check if they match
    assert data == expected

    # Delete output json xs file after check
    if os.path.exists('PinXS.json'):
        os.remove('PinXS.json')
    else:
        print('Attempted to delete JSON output file but it does not exist')


def test_openmc_xs():
    """Testing moltres_xs.py with OpenMC XS data, compared with the expected
    JSON file in the gold directory
    """

    # Run moltres_xs with OpenMC data
    os.system('python ../moltres_xs.py godiva/godiva_openmc.inp')

    # Load the output json xs file
    with open('godiva/godiva_openmc.json') as f:
        data = json.load(f)

    # Load gold copy of json xs file
    with open('gold/godiva.json') as h:
        expected = json.load(h)

    # Check if they match
    assert data == expected

    # Delete output json xs file after check
    if os.path.exists('godiva/godiva_openmc.json'):
        os.remove('godiva/godiva_openmc.json')
    else:
        print('Attempted to delete JSON output file but it does not exist')
