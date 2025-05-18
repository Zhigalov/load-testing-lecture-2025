#include "post.hpp"

#include <string>

namespace clck {

namespace {

std::string UrlToHash(const std::string& url) {
    static const std::string kAlphabet{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"};
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
  static const std::string kDomain{"http://clck.ru/"};

  userver::formats::json::ValueBuilder result;
  result["short_url"] = kDomain + UrlToHash(json["url"].As<std::string>());

  return result.ExtractValue();
}

}  // namespace clck