Appears that keff is far lower than it should be, around 0.02. Going to be looking into this.

Looks like we're using CHI_T for the prompt neutrons. Really, we should be using CHI_P and CHI_D data separately. CHI_T should be calculated
internal to moltres. Moreover, since this app is focused on fluid fuel, there's almost never a point in using CHI_T alone (ie stationary fuel, steady state)

Double checked: SP0 matrix indeed is not being read as transposed. Eigenvalue still too low.
