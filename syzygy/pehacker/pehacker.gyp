# Copyright 2013 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'pehacker_lib',
      'type': 'static_library',
      'sources': [
        'operation.h',
        'pehacker_app.cc',
        'pehacker_app.h',
        'variables.cc',
        'variables.h',
        'operations/add_imports_operation.cc',
        'operations/add_imports_operation.h',
        'operations/redirect_imports_operation.cc',
        'operations/redirect_imports_operation.h',
      ],
      'dependencies': [
        '<(src)/base/base.gyp:base',
        '<(src)/syzygy/application/application.gyp:application_lib',
        '<(src)/syzygy/block_graph/analysis/block_graph_analysis.gyp:'
            'block_graph_analysis_lib',
        '<(src)/syzygy/block_graph/transforms/block_graph_transforms.gyp:'
            'block_graph_transforms_lib',
        '<(src)/syzygy/common/common.gyp:common_lib',
        '<(src)/syzygy/pe/orderers/pe_orderers.gyp:pe_orderers_lib',
        '<(src)/syzygy/pe/pe.gyp:pe_lib',
        '<(src)/syzygy/pe/transforms/pe_transforms.gyp:pe_transforms_lib',
        '<(src)/syzygy/relink/relink.gyp:relink_lib',
      ],
    },
    {
      'target_name': 'pehacker',
      'type': 'executable',
      'sources': [
        'pehacker_main.cc',
        'pehacker.rc',
      ],
      'dependencies': [
        'pehacker_lib',
      ],
    },
    {
      'target_name': 'pehacker_unittest_utils',
      'type': 'static_library',
      'sources': [
        'unittest_util.cc',
        'unittest_util.h',
      ],
      'dependencies': [
        '<(src)/base/base.gyp:base',
        '<(src)/testing/gmock.gyp:gmock',
        '<(src)/testing/gtest.gyp:gtest',
        '<(src)/syzygy/core/core.gyp:core_unittest_utils',
        '<(src)/syzygy/pdb/pdb.gyp:pdb_unittest_utils',
        '<(src)/syzygy/pe/pe.gyp:pe_unittest_utils',
      ],
    },
    {
      'target_name': 'pehacker_unittests',
      'type': 'executable',
      'sources': [
        'pehacker_app_unittest.cc',
        'variables_unittest.cc',
        'operations/add_imports_operation_unittest.cc',
        'operations/redirect_imports_operation_unittest.cc',
        '<(src)/base/test/run_all_unittests.cc',
      ],
      'dependencies': [
        'pehacker_lib',
        'pehacker_unittest_utils',
        '<(src)/base/base.gyp:base',
        '<(src)/base/base.gyp:test_support_base',
        '<(src)/testing/gmock.gyp:gmock',
        '<(src)/testing/gtest.gyp:gtest',
        '<(src)/syzygy/core/core.gyp:core_unittest_utils',
        '<(src)/syzygy/pdb/pdb.gyp:pdb_unittest_utils',
        '<(src)/syzygy/pe/pe.gyp:pe_unittest_utils',
        '<(src)/syzygy/pe/pe.gyp:test_dll',
      ],
    },
  ],
}
