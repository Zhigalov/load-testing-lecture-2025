#include "post.hpp"

namespace clck {

HandlerCreateUrl::HandlerCreateUrl(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {}

std::string HandlerCreateUrl::HandleRequestThrow(
    const userver::server::http::HttpRequest&,
    userver::server::request::RequestContext&) const {
  return "Ok";
}

}  // namespace clck