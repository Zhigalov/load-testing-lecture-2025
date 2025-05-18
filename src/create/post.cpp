#include "post.hpp"

namespace clck {

HandlerCreateUrl::HandlerCreateUrl(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {}

userver::formats::json::Value HandlerCreateUrl::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest&,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext&) const {
  userver::formats::json::ValueBuilder result;
  result["short_url"] = "Ok";

  return result.ExtractValue();
}

}  // namespace clck