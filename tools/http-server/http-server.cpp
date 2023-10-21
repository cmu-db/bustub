#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>
#include "binder/table_ref/bound_join_ref.h"
#include "catalog/column.h"
#include "catalog/schema.h"
#include "common/bustub_instance.h"
#include "common/exception.h"
#include "cpp-httplib/httplib.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/comparison_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/nested_loop_join_plan.h"
#include "execution/plans/projection_plan.h"
#include "execution/plans/seq_scan_plan.h"
#include "type/type.h"
#include "type/type_id.h"

using json = nlohmann::json;

/*
auto TransformType(const std::string &ft) -> bustub::TypeId {
  if (ft == "INTEGER") {
    return bustub::TypeId::INTEGER;
  }
  if (ft == "BOOLEAN") {
    return bustub::TypeId::BOOLEAN;
  }
  throw bustub::Exception("unsupported field type");
}

auto TransformExpr(json expr, const std::vector<bustub::AbstractPlanNodeRef> &children)
    -> bustub::AbstractExpressionRef {
  bustub::TypeId out_type = TransformType(expr["outType"]);
  if (expr.contains("op")) {
    json op = expr["op"];
    std::string name = op["name"];
    std::vector<json> operands_json = expr["operands"];
    std::vector<bustub::AbstractExpressionRef> operands;
    operands.reserve(operands_json.size());
    for (const auto &operand_json : operands_json) {
      operands.emplace_back(TransformExpr(operand_json, children));
    }
    if (name == "=") {
      return std::make_shared<bustub::ComparisonExpression>(operands[0], operands[1], bustub::ComparisonType::Equal);
    }
    throw bustub::Exception("unsupported op");
  }

  // column value expression
  std::string name = expr["name"];
  size_t input = expr["input"];
  size_t i = 0;
  for (; i < children.size(); i++) {
    size_t cnt = children[i]->OutputSchema().GetColumnCount();
    if (input < cnt) {
      break;
    }
    input -= cnt;
  }
  return std::make_shared<bustub::ColumnValueExpression>(i, input, out_type);
}

auto TransformRootRel(bustub::BustubInstance &bustub, const std::map<std::string, json> &rels, json root_rel)
    -> bustub::AbstractPlanNodeRef {
  std::string rel_op = root_rel["relOp"];
  std::vector<std::string> inputs = root_rel["inputs"];
  std::vector<bustub::AbstractPlanNodeRef> input_plan_nodes;
  for (const auto &input : inputs) {
    auto input_rel = rels.find(input)->second;
    auto input_plan_node = TransformRootRel(bustub, rels, input_rel);
    input_plan_nodes.emplace_back(std::move(input_plan_node));
  }
  std::vector<std::string> fields = root_rel["fields"];
  std::vector<std::string> field_types = root_rel["fieldTypes"];
  std::vector<bustub::Column> columns;
  for (size_t i = 0; i < fields.size(); i++) {
    auto ft = field_types[i];
    columns.emplace_back(fields[i], TransformType(ft));
  }
  bustub::SchemaRef schema = std::make_shared<bustub::Schema>(columns);
  if (rel_op == "org.apache.calcite.interpreter.Bindables$BindableJoin") {
    std::string join_type = root_rel["joinType"];
    bustub::JoinType jt;
    if (join_type == "inner") {
      jt = bustub::JoinType::INNER;
    } else {
      throw bustub::Exception("unsupported join type");
    }
    auto predicate = TransformExpr(root_rel["condition"], input_plan_nodes);
    return std::make_shared<bustub::NestedLoopJoinPlanNode>(std::move(schema), input_plan_nodes[0], input_plan_nodes[1],
                                                            predicate, jt);
  }
  if (rel_op == "org.apache.calcite.interpreter.Bindables$BindableTableScan") {
    std::string table_name = root_rel["table"][0];
    auto table_info = bustub.catalog_->GetTable(table_name);
    return std::make_shared<bustub::SeqScanPlanNode>(std::move(schema), table_info->oid_, table_name);
  }
  if (rel_op == "org.apache.calcite.interpreter.Bindables$BindableProject") {
    std::vector<bustub::AbstractExpressionRef> exprs;
    std::vector<json> exprs_json = root_rel["exprs"];
    exprs.reserve(exprs_json.size());
    for (const auto &expr_json : exprs_json) {
      exprs.emplace_back(TransformExpr(expr_json, input_plan_nodes));
    }
    return std::make_shared<bustub::ProjectionPlanNode>(std::move(schema), exprs, input_plan_nodes[0]);
  }
  throw bustub::Exception("unsupported rel type");
}
*/

auto BuildPlanNodeFromJson(bustub::BustubInstance &bustub, json plan) -> bustub::AbstractPlanNodeRef {
  std::map<std::string, json> rels;
  std::vector<json> json_rels = plan["rels"];
  for (const auto &rel : json_rels) {
    rels[rel["id"]] = rel;
  }
  json root_rel = *json_rels.rbegin();
  // return TransformRootRel(bustub, rels, root_rel);
  return nullptr;
}

// NOLINTNEXTLINE
auto main(int argc, char **argv) -> int {
  auto bustub = std::make_unique<bustub::BustubInstance>();

  // HTTP
  httplib::Server svr;

  svr.set_exception_handler([](const auto &req, auto &res, std::exception_ptr ep) {
    std::string exception;
    try {
      std::rethrow_exception(ep);
    } catch (bustub::Exception &e) {
      exception = e.what();
    } catch (std::exception &e) {
      exception = e.what();
    } catch (...) {  // See the following NOTE
      exception = "unknown exception";
    }
    res.set_content(exception, "text/plain");
    res.status = 500;
  });

  svr.Post("/sql", [&bustub](const httplib::Request &req, httplib::Response &res) {
    std::stringstream ss;
    bustub::SimpleStreamWriter writer(ss, false);
    json data = json::parse(req.body);
    std::cerr << "SQL request: " << data["sql"] << std::endl;
    bustub->ExecuteSql(data["sql"], writer);
    res.set_content(ss.str(), "text/plain");
  });

  svr.Post("/plan", [&bustub](const httplib::Request &req, httplib::Response &res) {
    std::stringstream ss;
    bustub::SimpleStreamWriter writer(ss, false);
    std::cerr << "Plan request:";
    json json_plan = json::parse(req.body);
    std::cerr << json_plan << std::endl;
    auto plan = BuildPlanNodeFromJson(*bustub, json_plan);
    if (plan == nullptr) {
      std::cerr << "unsupported plan" << std::endl;
      res.set_content("unsupported plan", "text/plain");
      res.status = 500;
    } else {
      std::cerr << "BusTub plan:" << std::endl << plan->ToString(true) << std::endl;
      bustub->ExecutePlan(plan, writer);
      res.set_content(ss.str(), "text/plain");
    }
  });

  svr.Get("/catalog", [&bustub](const httplib::Request &req, httplib::Response &res) {
    std::cerr << "Catalog request" << std::endl;
    auto tables = bustub->catalog_->GetTableNames();
    std::vector<json> catalog;
    for (const auto &tbl_name : tables) {
      auto table = bustub->catalog_->GetTable(tbl_name);
      std::vector<json> schema;
      for (size_t c = 0; c < table->schema_.GetColumnCount(); c++) {
        auto col = table->schema_.GetColumn(c);
        switch (col.GetType()) {
          case bustub::TypeId::BIGINT: {
            schema.emplace_back(std::map<std::string, std::string>{{"name", col.GetName()}, {"type", "bigint"}});
            break;
          }
          case bustub::TypeId::INTEGER: {
            schema.emplace_back(std::map<std::string, std::string>{{"name", col.GetName()}, {"type", "integer"}});
            break;
          }
          case bustub::TypeId::VARCHAR: {
            schema.emplace_back(std::map<std::string, std::string>{{"name", col.GetName()}, {"type", "varchar"}});
            break;
          }
          default:
            throw bustub::Exception("unsupported column type");
        }
      }
      catalog.emplace_back(std::map<std::string, json>{{std::string("name"), json(table->name_)},
                                                       {std::string("oid"), json(table->oid_)},
                                                       {std::string("schema"), json(schema)}});
    }
    res.set_content(json(std::map<std::string, json>{{"catalog", catalog}}).dump(), "text/plain");
  });

  std::cerr << "BusTub HTTP server listening" << std::endl;

  svr.listen("127.0.0.1", 23333);

  return 0;
}
