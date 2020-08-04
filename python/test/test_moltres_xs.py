import json
import argparse
import numpy as np
from mock import patch, mock_open
from pyne import serpent
import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from moltres_xs import *


def test_serpent_xs():
    """Testing moltres_xs.py with Serpent 2 XS data, compared
    with the expected JSON file in the gold directory
    """

    # Run moltres_xs with Serpent 2 data
    read_input('msfr_xs.inp')

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
    read_input('pin_cell_XS.inp')

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
