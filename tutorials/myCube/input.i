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
    mesh: myCube.exo

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
        target_name: block_1-TETRA
        value:
         temperature: 10.0

    material_properties:
      target_name: block_1-TETRA
      specifications:
        - name: thermal_conductivity
          type: constant
          value: 1.0

    boundary_conditions:

    - wall_boundary_condition: bc_1
      target_name: surface_1
      wall_user_data:
        temperature: 20.0

    - wall_boundary_condition: bc_2
      target_name: surface_2
      wall_user_data:
        temperature: 20.0

    - wall_boundary_condition: bc_3
      target_name: surface_3
      wall_user_data:
        temperature: 40.0

    - wall_boundary_condition: bc_4
      target_name: surface_4
      wall_user_data:
        temperature: 40.0

    - wall_boundary_condition: bc_5
      target_name: surface_5
      wall_user_data:
        temperature: 50.0

    - wall_boundary_condition: bc_6
      target_name: surface_6
      wall_user_data:
        temperature: 50.0

    solution_options:
      name: myOptions

      use_consolidated_solver_algorithm: yes

      options:
      - element_source_terms:
          temperature: FEM_DIFF

    output:
      output_data_base_name: femHC.e
      output_frequency: 10
      output_node_set: no 
      output_variables:
       - dual_nodal_volume
       - temperature

Time_Integrators:
  - StandardTimeIntegrator:
      name: ti_1
      start_time: 0
      termination_step_count: 20
      time_step: 10.0 
      time_stepping_type: fixed
      time_step_count: 0
      second_order_accuracy: no

      realms:
        - realm_1
