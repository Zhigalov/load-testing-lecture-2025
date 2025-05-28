#include "get.hpp"

namespace clck {

HandlerFetchUrl::HandlerFetchUrl(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {}

std::string HandlerFetchUrl::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  const auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT url FROM clck_schema.urls WHERE hash = $1 AND version = "
      "$2::INTEGER",
      request.GetPathArg("hash"), request.GetPathArg("version"));

  if (result.IsEmpty()) {
    auto& response = request.GetHttpResponse();
    response.SetStatus(userver::server::http::HttpStatus::kNotFound);

    return "Url not found";
  }

  return result.AsSingleRow<std::string>();
}

}  // namespace clck