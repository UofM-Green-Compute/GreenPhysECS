# **GreenPhysECS — Exploring ECS for Physics Simulation with Energy Tracking**

**GreenPhysECS** is an **experimental exploratory codebase** developed to answer the research question:

**Can the Entity–Component–System (ECS) model make building physics simulations simpler and more parallel-friendly for early career researchers?**

The repository provides a containerised C++20 environment, a snapshot of the **Flecs** ECS framework, an **energy-tracking subsystem**, and a set of simulation sketches created as part of an **MPhys project** at the University of Manchester.

This work sits at the intersection of:

* **physics simulation**  
* **software architecture for scientific code**  
* **parallelism & data-oriented design**  
* **reproducible research**  
* **green/energy-aware computing**

The current stage of the project is **exploratory**: the aim is to determine whether ECS is a viable and helpful architectural pattern for physics before investing in a production-quality implementation.

## **Motivation**

Physics workloads are often naturally parallel, yet physics simulations written by novices frequently end up highly linear and difficult to port to multi-core or GPU environments later.

The **Entity–Component–System** model — widely used in games — applies **simple local rules across many interacting entities concurrently**, making data-parallel simulation natural rather than bolted on afterwards. Unlike object-oriented designs, ECS separates:

* **entities** — identity  
* **components** — data  
* **systems** — behaviour

This project investigates whether this pattern can:

* make simulation code clearer for novices,  
* improve decomposability into data-parallel updates,  
* ease future GPU or cluster offloading,  
* and support reproducible, energy-measurable research workflows.

## **Project Context**

This repository was developed within the **MPhys project programme** at Manchester. Two students worked on the code, supervised by a SRSE. The overarching objective was not to deliver a “finished simulation framework”, but to **test the viability of the ECS approach** for physics and document early experiences and dead-ends.

The project proposal emphasises that ECS has been used for “game physics” but rarely for real physics problems, with AtomECS as one notable exception

A parallel motivation was to integrate **energy measurement** into simulation workflows to support research on **sustainable and carbon-aware computing**, aligning with wider work in **EVERSE** and **Green Compute**.


## **What This Repository Contains**

The repository includes:

* an **ECS framework snapshot** (Flecs v4.1.1)  
* a **C++20 build environment** (Docker-containerised)  
* an **energy tracking subsystem** (RAPL-based)  
* **simulation sketches and examples** (“gravity”, fluids, harmonics, etc.)  
* a simple **batch execution model** for energy measurement  
* **student-friendly onboarding** materials

The development process followed a **“sketch → example → maybe infrastructure”** flow, promoting working exploratory code and documenting reasoning and dead-ends.


## **What This Repository Does *Not* Claim**

To avoid confusion:

* it is **not** a general-purpose physics framework  
* it is **not** optimised or tuned for performance  
* it is **not** multi-threaded (yet)  
* it is **not** wired into GPUs or HPC schedulers (yet)  
* it is **not** a demonstration of state-of-the-art ECS technique

Those directions are **future considerations**, contingent on exploratory results being positive.

## **Research Questions**

The primary questions being explored are:

1. **Architectural usability** — Can novices express physics more cleanly in ECS?  
2. **Parallelism** — Does ECS encourage data-parallel thinking early?  
3. **Portability** — Does ECS structure map naturally to GPU/HPC?  
4. **Reproducibility** — Does a containerised environment help supervision and assessment?  
5. **Energy awareness** — Can energy measurement be integrated into normal simulation workflows?

A secondary question is sustainability-driven:

**If simulation cost can be measured, can it eventually be predicted or scheduled carbon-aware?**

## **Build & Run**

### **Requirements**

The build environment is a containerised build setup.  Core Requirements:

* docker  
* git  
* make

### **Containerised build (reproducible)**

First build your build container:

* make docker

Build (builds inside container)

* make dockerbuild

Run (runs inside container)

* make run

To rummage around inside the container if unexpected things happen:

* make dockerbash

The Docker image pins OS \+ toolchain versions for consistency across student laptops, clusters, and supervision machines.

## **Energy Measurement**

Energy tracking uses Intel’s **RAPL** interface and was executed in **batch mode** on a dedicated Linux workstation to ensure clean measurements.

Outputs include:

* per-run energy consumption  
* time-series logs  
* CSV traces suitable for plotting/analysis

This was used to support investigations into **greener research computing** and to explore **carbon-aware scheduling** ideas.

## **Repository Structure**

`include/     # Energy tracking library + flecs includes`  
`src/         # Example/basic usages`  
`Sketches/    # exploratory prototypes and ideas`  
`examples/    # demonstration simulations extracted from Sketches`  
`docs/        # notes, onboarding, and context`  
`Dockerfile   # pinned execution environment`  
`Makefile     # Core build targets`

## **Current Status**

The project is **active when students select it**, and maintained by a **Senior Research Software Engineer (SRSE)** between cohorts. Future investment depends on whether the ECS approach continues to show promise; if it does not, the project will naturally conclude.

## **Roadmap (Conditional)**

If ECS proves sufficiently useful, potential future directions include:

* multi-threaded execution  
* GPU back-ends (CUDA / SYCL / Vulkan / WebGPU)  
* integration into HPC schedulers  
* CI/CD \+ tests  
* example library of physics domains  
* notebook-friendly visualisation interfaces  
* carbon-estimation models for pre-run scheduling  
* teaching/tutorial material for ECS in physics

## **Licensing & Citation**

Apache 2 Licensing \- contributions welcome.

Zenodo archival and short academic write-up are anticipated.

If you build on this, please cite the repository and supervisory context.

## Note on AI use

***Please note:** this README has been through an AI editor to improve the language and scope. The content has been checked for correctness - if you spot any errors, please open an issue.*

*Gen AI is generally NOT used in this repo \- since it explores human usability of ECS.*
