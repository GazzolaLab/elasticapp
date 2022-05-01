\cond NEVER Distributed under the MIT License. See LICENSE.txt for details. \endcond

\mainpage Introduction

<div class="toc">
Table of Contents
<ul>
<li class="level1"><a href="#intro_sec">Introduction</a></li>
</ul>
</div>

# What is Elastica++? {#intro_sec}

`Elastica++` is a `C++` package to simulate the dynamics of soft-filaments that, at every cross-section, can undergo all
six possible modes of deformation, allowing the filament to bend, twist, stretch and shear. Such slender structures are
ubiquitous in natural and artificial systems across scales, from DNA strands to flagella and space tethers. They also
make up a variety of complex systems, from birds’ nests made of twigs to living creatures made of bones, tendons, fibers
and muscles.

`Elastica++` models these filaments using Cosserat rod theory which allows for modeling assemblies of such rods while
interacting with complex exogenous (surface contact, friction and hydrodynamics) and endogenous (muscular activity)
effects. `Elastica++` maps such assemblies efficiently to multicore processors using a novel Block architecure which
allows it to scale from laptops to computing clusters. More information about `Elastica++` is available at the
Elastica [project website](https://cosseratrods.org)