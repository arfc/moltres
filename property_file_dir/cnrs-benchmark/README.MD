# Multigroup cross-section & precursor data for the CNRS benchmark

The ```feedback.py``` script was used to apply salt thermal expansion feedback
on the relevant diffusion coefficients and cross-sections (DIFFCOEF, FISS, NSF,
REMXS, & SP0) based on the formula provided by Tiberga et al. [1].

```benchmark_CHIT.txt``` is a duplicate of ```benchmark_CHIP.txt``` because
Moltres requires CHIT input data even if it doesn't use them in any
calculations.

## References

[1] Tiberga et al., "Results from a multi-physic numerical benchmark for codes
dedicated to molten salt fast reactors," Annals of Nuclear Energy, vol. 142,
July 2020, 107428.
