import json
import argparse
import numpy as np
from mock import patch, mock_open
from pyne import serpent
import sys, os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from moltres_xs import *


def test_serpent_xs():
    """Testing moltres_xs.py with Serpent 2 XS data, compared
    with the expected JSON file in the gold directory
    """
    read_input('msfr_xs.inp')
    with open('msfrXS.json') as f:
        data = json.load(f)
    with open('gold/msfrXS.json') as h:
        expected = json.load(h)
    assert data == expected
    if os.path.exists('msfrXS.json'):
        os.remove('msfrXS.json')
    else:
        print('Attempted to delete JSON output file but it does not exist')


def test_scale_xs():
    """Testing moltres_xs.py with SCALE XS data, compared
    with the expected JSON file in the gold directory
    """
    read_input('pin_cell_XS.inp')
    with open('PinXS.json') as f:
        data = json.load(f)
    with open('gold/PinXS.json') as h:
        expected = json.load(h)
    assert data == expected
    if os.path.exists('PinXS.json'):
        os.remove('PinXS.json')
    else:
        print('Attempted to delete JSON output file but it does not exist')

