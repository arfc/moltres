# Multigroup cross-section & precursor data for the CNRS benchmark

```feedback.py``` applies the salt thermal expansion feedback on the relevant
diffusion coefficients and cross-sections based on the formula provided by
Tiberga et al. [1]. Run the script using the following command:

```
$ python3 feedback.py
```

```benchmark_CHIT.txt``` is a duplicate of ```benchmark_CHIP.txt``` because
Moltres requires CHIT cross sections even if it doesn't use them.

## References

[1] Tiberga et al., "Results from a multi-physic numerical benchmark for codes
dedicated to molten salt fast reactors," Annals of Nuclear Energy, vol. 142,
July 2020, 107428.
