---
title: 'Moltres: finite element based simulation of molten salt reactors'
tags:
  - finite elements
  - molten salt reactors
  - neutron diffusion
authors:
 - name: Alexander Lindsay
   orcid: 0000-0002-6988-2123
   affiliation: 1
 - name: Kathryn Huff
   orcid: 0000-0002-7075-6802
   affiliation: 1
affiliations:
 - name: University of Illinois, Urbana-Champaign
   index: 1
date: 24 May 2017
bibliography: paper.bib
---

# Summary

Moltres is a physics application for multiphysics modeling of fluid-fuelled
molten salt reactors (MSRs). It couples equations for neutron diffusion, thermal
hydraulics, and delayed neutron precursor transport. Neutron diffusion and
precursor transport equations are set-up using an action system that allows the
user to use an arbitrary number of neutron energy and precursor groups
respectively with minimal input changes. Moltres sits on top of the
Multi-physics Object-Oriented Simulation Environment
[@gaston_physics-based_2015] which gives it the capability to run seamlessly in
massively parallel environments. To date Moltres has been used to simulate MSRs
in 2D-axisymmetric and 3D geometric configurations.

# References
