## Citations

The information below should help you to properly cite Moltres where appropriate.
Up-to-date citations will be added as they are produced. 

### Software Citation

If you use the Moltres application, we ask that you indicate the version of Moltres that you used with a git hash and cite the closest version as below::


#### Version 0.3.0

```tex
@article{lindsay_moltres:_2018,
    title = {Moltres: finite element based simulation of molten salt reactors},
    volume = {3},
    shorttitle = {Moltres},
    url = {https://doi.org/10.21105/joss.00298},
    doi = {10.21105/joss.00298},
    number = {21},
    urldate = {2018-01-08},
    journal = {The Journal of Open Source Software},
    author = {Lindsay, Alexander and Huff, Kathryn},
    month = jan,
    year = {2018},
    note = {DOI: 10.21105/joss.00298},
    pages = {1--2}
}
```



### Method and Results Citations

If you reference any of our published results or methods, please also cite the
relevant paper(s) below.  

The fundamental methods can be cited via the Introduction to Moltres paper:

```tex
@article{lindsay_introduction_2018,
    title = {Introduction to {Moltres}: {An} application for simulation of {Molten} {Salt} {Reactors}},
    volume = {114},
    issn = {0306-4549},
    shorttitle = {Introduction to {Moltres}},
    url = {https://www.sciencedirect.com/science/article/pii/S0306454917304760},
    doi = {10.1016/j.anucene.2017.12.025},
    abstract = {Moltres is a new physics application for modeling coupled physics in fluid-fuelled, molten salt reactors. This paper describes its neutronics model, thermal hydraulics model, and their coupling in the MOOSE framework. Neutron and precursor equations are implemented using an action system that allows use of an arbitrary number of groups with no change in the input card. Results for many-channel configurations in 2D-axisymmetric and 3D coordinates are presented and compared against other coupled models as well as the Molten Salt Reactor Experiment.},
    urldate = {2018-01-08},
    journal = {Annals of Nuclear Energy},
    author = {Lindsay, Alexander and Ridley, Gavin and Rykhlevskii, Andrei and Huff, Kathryn},
    month = apr,
    year = {2018},
    keywords = {agent based modeling, Finite elements, Hydrologic contaminant transport, Molten Salt Reactors, MOOSE, Multiphysics, nuclear engineering, Nuclear fuel cycle, Object orientation, Parallel computing, Reactor physics, repository, Simulation, Systems analysis},
    pages = {530--540},
}
```

To cite Gavin Ridley's presentation on MSR Transients with Moltres, cite the following report:

```tex
@techreport{ridley_multiphysics_2017,
    address = {Urbana, IL},
    type = {Undergraduate {Report}},
    title = {Multiphysics {Analysis} of {Molten} {Salt} {Reactor} {Transients}},
    url = {https://github.com/arfc/publications/tree/2017-ridley-msrTransients},
    abstract = {Molten salt nuclear reactor technology has not yet been constructed for industrial scale. High fidelity simulation capability of both transients and steady-state behavior must be developed for reactor licensing. The simulations should make use of high performance computing (HPC) in order
to come to realistic results while minimizing assumptions. High resolution simulation of limiting reactor transients using
open-source software can inform reproducible results suitable for preliminary licensing activity. We present example results of the new code.},
    number = {UIUC-ARFC-2017-01},
    institution = {University of Illinois at Urbana-Champaign},
    author = {Ridley, Gavin},
    month = aug,
    year = {2017},
    pages = {0--12}
}
```
