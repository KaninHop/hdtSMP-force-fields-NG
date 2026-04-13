#include "PluginHelper.h"
#include "hooks.h"

// SKSE_EXPORT = extern "C" [[maybe_unused]] __declspec(dllexport)
// Compatible with all CommonLibSSE-NG forks (powerof3, CharmedBaryon, alandtse, …).
SKSE_EXPORT constinit auto SKSEPlugin_Version = []() noexcept {
	SKSE::PluginVersionData v;
	v.PluginVersion({ 0, 9, 1, 0 });
	v.PluginName("HDT-SMP Force Fields");
	v.AuthorName("jgernandt");
	v.UsesAddressLibrary();
	v.UsesNoStructs();
	return v;
}();

static void InitializeLog(std::string_view pluginName)
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		util::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= std::format("{}.log"sv, pluginName);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
	const auto level = spdlog::level::trace;
#else
	const auto level = spdlog::level::info;
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
	log->set_level(level);
	log->flush_on(level);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] [%s:%#] %v");
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	const auto  name = SKSEPlugin_Version.GetPluginName();
	const auto  version = SKSEPlugin_Version.GetPluginVersion();

	InitializeLog(name);

	logger::info("{} {} is loading...", name, version);

	SKSE::Init(a_skse);

	logger::info("Attaching engine hooks...");
	if (Hooks::Install()) {
		logger::info("Engine hooks attached");
	}
	else {
		logger::error("Failed to attach engine hooks.");
		logger::info("Initialisation failed.");
		return false;
	}

	logger::info("Connecting to HDT-SMP...");
	hdt::PluginHelper::tryConnect(a_skse);

	return true;
}
