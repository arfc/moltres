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

Moltres is a physics application for multiphysics modeling of fluid-fueled
molten salt reactors (MSRs) [@moltres]. It couples equations for neutron diffusion, thermal
hydraulics, and delayed neutron precursor transport. Neutron diffusion and
precursor transport equations are set-up using an action system that allows the
user to use an arbitrary number of neutron energy and precursor groups
respectively with minimal input changes. Moltres sits on top of the
Multi-physics Object-Oriented Simulation Environment
[@moose] and hence uses the finite element method to discretize the governing
partial differential equations. In general the resulting system of non-linear
algebraic equations is linearized using the Newton-Raphson method and then
solved using the Portable, Extensible Toolkit for Scientific Computation
[@petsc]. Assembly of the Jacobian and residual, and the linear solve are
parallelized using MPI which allows Moltres to be run in
massively parallel environments. Runs on the Blue Waters supercomputer at
Illinois have utilized up to 608 cores.

Moltres and MOOSE allow use of different basis functions for different system
variables. Because of the purely diffusive nature of the neutron diffusion
equations, neutron fluxes are typically discretized using continuous
first-degree Lagrange polynomials and the degrees of freedom are associated with
mesh nodes. The temperature variable may also be
discretized with a continuous Lagrange basis, or a discontinuous basis of
arbitrary degree monomials may be employed depending on the relative balance of
heat convection to conduction. The purely hyperbolic precursor transport is
currently discretized using constant monomials, which is equivalent to a first-order
finite volume discretization. Moltres supports both segregated (through Picard
iteration) and monolithic
solutions of the equation system. However, due to the feedback between the
power spectrum and temperature dependence of macroscopic cross-sections,
monolithic solves have demonstrated superior robustness with segregated
techniques often unable to converge to a solution. This result emphasizes the
importance of a fully coupled multi-physics framework like the one that Moltres and MOOSE
provide and suggests that iteratively coupling codes devoted to single physics
[@kophazi_development_2009] may result in limited flexibility.

# References
