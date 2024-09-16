# Copyright 2019-2020 CERN and copyright holders of ALICE O2.
# See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
# All rights not expressly granted are reserved.
#
# This software is distributed under the terms of the GNU General Public
# License v3 (GPL Version 3), copied verbatim in the file "COPYING".
#
# In applying this license CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization
# or submit itself to any jurisdiction.

o2physics_add_dpl_workflow(example1
					SOURCES Example1.cxx
#PUBLIC_LINK_LIBRARIES O2::Framework O2Physics::AnalysisCore
					PUBLIC_LINK_LIBRARIES O2::Framework
					COMPONENT_NAME HFO2HackTut)

o2physics_add_dpl_workflow(example2
					SOURCES Example2.cxx
					PUBLIC_LINK_LIBRARIES O2::Framework
					COMPONENT_NAME HFO2HackTut)

o2physics_add_dpl_workflow(example3
					SOURCES Example3.cxx
					PUBLIC_LINK_LIBRARIES O2::Framework
					COMPONENT_NAME HFO2HackTut)

o2physics_add_dpl_workflow(example3sub1
					SOURCES Example3_1.cxx
					PUBLIC_LINK_LIBRARIES O2::Framework
					COMPONENT_NAME HFO2HackTut)

o2physics_add_dpl_workflow(example4
					SOURCES Example4.cxx
					PUBLIC_LINK_LIBRARIES O2::Framework
					COMPONENT_NAME HFO2HackTut)

o2physics_add_dpl_workflow(example5
					SOURCES Example5.cxx
					PUBLIC_LINK_LIBRARIES O2::Framework O2Physics::AnalysisCore
					COMPONENT_NAME HFO2HackTut)

o2physics_add_dpl_workflow(example5sub1
					SOURCES Example5_1.cxx
					PUBLIC_LINK_LIBRARIES O2::Framework O2Physics::AnalysisCore
					COMPONENT_NAME HFO2HackTut)

o2physics_add_dpl_workflow(example6
					SOURCES Example6.cxx
					PUBLIC_LINK_LIBRARIES O2::Framework O2Physics::AnalysisCore
					COMPONENT_NAME HFO2HackTut)

o2physics_add_dpl_workflow(example7
					SOURCES Example7.cxx
					PUBLIC_LINK_LIBRARIES O2::Framework O2Physics::AnalysisCore
					COMPONENT_NAME HFO2HackTut)

o2physics_add_dpl_workflow(practice1
					SOURCES Practice1.cxx
					PUBLIC_LINK_LIBRARIES O2::Framework O2Physics::AnalysisCore
					COMPONENT_NAME HFO2HackTut)
