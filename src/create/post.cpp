#include "post.hpp"

#include <string>
#include <userver/storages/postgres/cluster_types.hpp>

namespace clck {

namespace {

std::string UrlToHash(const std::string& url) {
  if (url == "collision1" || url == "collision2") {
    return "AAAA";
  }

  static const std::string kAlphabet{
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"};
  static const std::size_t kBase{kAlphabet.size()};
  static const std::size_t kResultLength{4};

  std::hash<std::string> hasher;
  size_t hash = hasher(url);

  std::size_t max_value = 1;
  for (std::size_t i = 0; i < kResultLength; ++i) {
    max_value *= kBase;
  }
  hash %= max_value;

  std::string result(kResultLength, 'A');
  for (std::size_t i = 0; i < kResultLength; ++i) {
    result[kResultLength - 1 - i] = kAlphabet[hash % kBase];
    hash /= kBase;
  }

  return result;
}

}  // namespace

HandlerCreateUrl::HandlerCreateUrl(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {}

userver::formats::json::Value HandlerCreateUrl::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest&,
    const userver::formats::json::Value& json,
    userver::server::request::RequestContext&) const {
  const auto url = json["url"].As<std::string>();
  const auto hash = UrlToHash(url);
  const auto pg_result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "WITH stored AS ("
      "       SELECT COALESCE(MAX(version), 0) AS max_version "
      "       FROM clck_schema.urls WHERE hash = $2 "
      "     ),"
      "     inserted AS ("
      "       INSERT INTO clck_schema.urls(url, hash, version) "
      "       SELECT $1, $2, stored.max_version + 1 FROM stored "
      "       ON CONFLICT DO NOTHING "
      "       RETURNING version "
      "     ) "
      "SELECT * FROM inserted UNION ALL "
      "SELECT version FROM clck_schema.urls WHERE url = $1 "
      "LIMIT 1;",
      url, hash);
  const auto version = pg_result.AsSingleRow<int>();

  userver::formats::json::ValueBuilder result;

  static const std::string kDomain{"http://158.160.81.197:8080/"};
  result["short_url"] = kDomain + hash + '/' + std::to_string(version);
  result["hash"] = hash;
  result["version"] = version;

  return result.ExtractValue();
}

}  // namespace clck