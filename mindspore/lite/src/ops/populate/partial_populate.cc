/**
 * Copyright 2019-2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "src/ops/partial.h"
#include "src/ops/primitive_c.h"
#include "src/ops/populate/populate_register.h"

namespace mindspore {
namespace lite {
typedef struct PartialParameter {
  OpParameter op_parameter_;
  int sub_graph_index_;
} PartialParameter;

OpParameter *PopulatePartialParameter(const mindspore::lite::PrimitiveC *primitive) {
  PartialParameter *partial_parameter = reinterpret_cast<PartialParameter *>(malloc(sizeof(PartialParameter)));
  if (partial_parameter == nullptr) {
    MS_LOG(ERROR) << "malloc partial parameter failed.";
    return nullptr;
  }
  memset(partial_parameter, 0, sizeof(PartialParameter));
  partial_parameter->op_parameter_.type_ = primitive->Type();

  auto param = reinterpret_cast<mindspore::lite::Partial *>(const_cast<mindspore::lite::PrimitiveC *>(primitive));
  partial_parameter->sub_graph_index_ = param->GetSubGraphIndex();

  return reinterpret_cast<OpParameter *>(partial_parameter);
}
Registry PartialParameterRegistry(schema::PrimitiveType_Partial, PopulatePartialParameter);
}  // namespace lite
}  // namespace mindspore
