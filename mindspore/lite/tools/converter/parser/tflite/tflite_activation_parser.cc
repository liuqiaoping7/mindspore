/**
 * Copyright 2020 Huawei Technologies Co., Ltd
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

#include "tools/converter/parser/tflite/tflite_activation_parser.h"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace mindspore {
namespace lite {
STATUS TfliteActivationParser::Parse(const std::unique_ptr<tflite::OperatorT> &tflite_op,
                                     const std::vector<std::unique_ptr<tflite::TensorT>> &tflite_tensors,
                                     const std::vector<std::unique_ptr<tflite::BufferT>> &tflite_model_buffer,
                                     schema::CNodeT *op,
                                     std::vector<int32_t> *tensors_id,
                                     std::vector<schema::Format> *tensors_format,
                                     std::map<int, int>  *tensors_id_map) {
  if (op == nullptr) {
    MS_LOG(ERROR) << "op is null";
    return RET_NULL_PTR;
  }
  op->primitive = std::make_unique<schema::PrimitiveT>();
  if (op->primitive == nullptr) {
    MS_LOG(ERROR) << "op->primitive is null";
    return RET_NULL_PTR;
  }
  std::unique_ptr<schema::ActivationT> attr(new schema::ActivationT());

  std::vector<std::string> node_name_str;
  Split(op->name, &node_name_str, "-");
  const char *node_name = node_name_str.data()->c_str();
  if (std::strcmp(node_name,  "Relu") == 0) {
    MS_LOG(DEBUG) << "parse TfliteReluParser";
    attr->type = schema::ActivationType_RELU;
  } else if (std::strcmp(node_name,  "Relu6") == 0) {
    MS_LOG(DEBUG) << "parse TfliteRelu6Parser";
    attr->type = schema::ActivationType_RELU6;
  } else if (std::strcmp(node_name,  "Tanh") == 0) {
    MS_LOG(DEBUG) << "parse TfliteTanhParser";
    attr->type = schema::ActivationType_TANH;
  } else if (std::strcmp(node_name,  "Logistic") == 0) {
    MS_LOG(DEBUG) << "parse TfliteLogisticParser";
    attr->type = schema::ActivationType_SIGMOID;
  } else if (std::strcmp(node_name,  "HardSwish") == 0) {
    MS_LOG(DEBUG) << "parse TfliteHardSwishParser";
    attr->type = schema::ActivationType_SIGMOID;
  }

  attr->alpha = 0.2f;
  op->primitive->value.type = schema::PrimitiveType_Activation;
  op->primitive->value.value = attr.release();

  AddOpInput(op, tensors_id, tensors_format, tensors_id_map,
             tflite_op->inputs[0], tensors_id->size(), tflite_tensors.size(), schema::Format_NHWC);
  AddOpOutput(op, tensors_id, tensors_format, tensors_id_map,
              tflite_op->outputs[0], tensors_id->size(), tflite_tensors.size(), schema::Format_NHWC);
  return RET_OK;
}

STATUS TflitePreluParser::Parse(const std::unique_ptr<tflite::OperatorT> &tflite_op,
                                const std::vector<std::unique_ptr<tflite::TensorT>> &tflite_tensors,
                                const std::vector<std::unique_ptr<tflite::BufferT>> &tflite_model_buffer,
                                schema::CNodeT *op,
                                std::vector<int32_t> *tensors_id,
                                std::vector<schema::Format> *tensors_format,
                                std::map<int, int>  *tensors_id_map) {
  MS_LOG(DEBUG) << "parse TflitePreluParser";

  if (op == nullptr) {
    MS_LOG(ERROR) << "op is null";
    return RET_NULL_PTR;
  }
  op->primitive = std::make_unique<schema::PrimitiveT>();
  if (op->primitive == nullptr) {
    MS_LOG(ERROR) << "op->primitive is null";
    return RET_NULL_PTR;
  }
  std::unique_ptr<schema::PreluT> attr(new schema::PreluT());

  if (GetTfliteData(tflite_op->inputs[1], tflite_tensors, tflite_model_buffer, attr->slope)) {
    MS_LOG(ERROR) << "get pRelu -> slope failed";
    return RET_ERROR;
  }
  op->primitive->value.type = schema::PrimitiveType_Prelu;
  op->primitive->value.value = attr.release();

  AddOpInput(op, tensors_id, tensors_format, tensors_id_map,
             tflite_op->inputs[0], tensors_id->size(), tflite_tensors.size(), schema::Format_NHWC);
  AddOpOutput(op, tensors_id, tensors_format, tensors_id_map,
              tflite_op->outputs[0], tensors_id->size(), tflite_tensors.size(), schema::Format_NHWC);
  return RET_OK;
}

STATUS TfliteLeakyReluParser::Parse(const std::unique_ptr<tflite::OperatorT> &tflite_op,
                                    const std::vector<std::unique_ptr<tflite::TensorT>> &tflite_tensors,
                                    const std::vector<std::unique_ptr<tflite::BufferT>> &tflite_model_buffer,
                                    schema::CNodeT *op,
                                    std::vector<int32_t> *tensors_id,
                                    std::vector<schema::Format> *tensors_format,
                                    std::map<int, int>  *tensors_id_map) {
  MS_LOG(DEBUG) << "parse TfliteLeakyReluParser";

  if (op == nullptr) {
    MS_LOG(ERROR) << "op is null";
    return RET_NULL_PTR;
  }
  op->primitive = std::make_unique<schema::PrimitiveT>();
  if (op->primitive == nullptr) {
    MS_LOG(ERROR) << "op->primitive is null";
    return RET_NULL_PTR;
  }

  std::unique_ptr<schema::LeakyReLUT> attr(new schema::LeakyReLUT());

  const auto &tflite_attr = tflite_op->builtin_options.AsLeakyReluOptions();
  if (tflite_attr == nullptr) {
    MS_LOG(ERROR) << "get op: " << op->name.c_str() << " attr failed";
    return RET_NULL_PTR;
  }
  attr->negativeSlope = tflite_attr->alpha;

  op->primitive->value.type = schema::PrimitiveType_LeakyReLU;
  op->primitive->value.value = attr.release();

  AddOpInput(op, tensors_id, tensors_format, tensors_id_map,
             tflite_op->inputs[0], tensors_id->size(), tflite_tensors.size(), schema::Format_NHWC);
  AddOpOutput(op, tensors_id, tensors_format, tensors_id_map,
              tflite_op->outputs[0], tensors_id->size(), tflite_tensors.size(), schema::Format_NHWC);
  return RET_OK;
}

TfliteNodeRegister g_TfliteReluParser("Relu", new TfliteReluParser());
TfliteNodeRegister g_TfliteRelu6Parser("Relu6", new TfliteRelu6Parser());
TfliteNodeRegister g_TfliteTanhParser("Tanh", new TfliteTanhParser());
TfliteNodeRegister g_TfliteHardSwishParser("HardSwish", new TfliteHardSwishParser());
TfliteNodeRegister g_tfliteLogisticParser("Logistic", new TfliteLogisticParser());
TfliteNodeRegister g_tflitePreluParser("Prelu", new TflitePreluParser());
TfliteNodeRegister g_TfliteLeakyReluParser("LeakyRelu", new TfliteLeakyReluParser());

}  // namespace lite
}  // namespace mindspore
