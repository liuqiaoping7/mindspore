# Copyright 2020 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================

"""Sum op"""
from mindspore.ops.op_info_register import op_info_register


@op_info_register("""{
    "op_name": "Sum",
    "imply_type": "AutoDiff",
    "fusion_type": "COMMREDUCE",
    "attr": [
        {
            "name": "axis",
            "param_type": "required",
            "type": "listInt"
        },
        {
            "name": "keepdims",
            "param_type": "required",
            "type": "bool"
        }
    ],
    "inputs": [
        {
            "index": 0,
            "dtype": [
                "float16", "float32", "float16", "float32",
                "float16", "float32"
            ],
            "format": [
                "DefaultFormat", "DefaultFormat", "NC1HWC0", "NC1HWC0",
                "FRACTAL_NZ", "FRACTAL_NZ"
            ],
            "param_type": "required",
            "name": "x"
        }
    ],
    "outputs": [
        {
            "index": 0,
            "dtype": [
                "float16", "float32", "float16", "float32",
                "float16", "float32"
            ],
            "format": [
                "DefaultFormat", "DefaultFormat", "NC1HWC0", "NC1HWC0",
                "FRACTAL_NZ", "FRACTAL_NZ"
            ],
            "name": "output"
        }
    ]
}""")
def _sum_akg():
    """Sum AutoDiff register"""
    return
