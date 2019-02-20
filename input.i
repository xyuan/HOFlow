# -*- mode: yaml -*-
#
# Example HOFlow input file for a heat conduction problem
#

Simulations:
  - name: sim1

linear_solvers:
  - name: solve_scalar
    type: tpetra
    method: gmres 
    preconditioner: sgs 
    tolerance: 1e-3
    max_iterations: 75 
    kspace: 75 
    output_level: 0

realms:
  - name: realm_1
    mesh: periodic3d.g

    equation_systems:
      name: theEqSys
      max_iterations: 2 
  
      solver_system_specification:
        temperature: solve_scalar
   
      systems:
        - HeatConduction:
            name: myHC
            max_iterations: 1
            convergence_tolerance: 1e-5

    initial_conditions:

      - constant: ic_1
        target_name: block_1
        value:
         temperature: 10.0

    material_properties:
      target_name: block_1
      specifications:
        - name: thermal_conductivity
          type: constant
          value: 1.0

    boundary_conditions:

    - wall_boundary_condition: bc_left
      target_name: surface_1
      wall_user_data:
        temperature: 20.0

    - wall_boundary_condition: bc_right
      target_name: surface_2
      wall_user_data:
        temperature: 40.0
