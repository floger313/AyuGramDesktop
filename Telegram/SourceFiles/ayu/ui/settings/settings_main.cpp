// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ayu/ui/settings/settings_main.h"

#include "settings/sections/settings_main.h"
#include "lang_auto.h"
#include "ayu/ayu_settings.h"
#include "ayu/ui/ayu_logo.h"
#include "ayu/ui/settings/settings_appearance.h"
#include "ayu/ui/settings/settings_ayu.h"
#include "ayu/ui/settings/settings_chats.h"
#include "ayu/ui/settings/settings_filters.h"
#include "ayu/ui/settings/settings_general.h"
#include "ayu/ui/settings/settings_other.h"
#include "core/version.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_ayu_settings.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/painter.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "apiwrap.h"
#include "window/window_session_controller.h"
#include "window/window_session_controller_link_info.h"
#include "main/main_session.h"
#include "ui/controls/userpic_button.h"
#include "data/data_user.h"
#include "data/data_session.h"
#include "styles/style_info.h"

#include <QDesktopServices>

namespace Settings {

using namespace Builder;

namespace {

void BuildLogo(SectionBuilder &builder) {
	builder.add([](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		const auto user = ctx.controller->session().user();
		auto button = object_ptr<Ui::UserpicButton>(
			ctx.container,
			ctx.controller,
			user,
			Ui::UserpicButton::Role::OpenPhoto,
			Ui::UserpicButton::Source::PeerPhoto,
			st::settingsInfoPhoto);
		return {
			.widget = object_ptr<Ui::RpWidget>(std::move(button)),
			.align = style::al_top,
		};
	});
}

void BuildVersionInfo(SectionBuilder &builder) {
	builder.add([](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		return {
			.widget = object_ptr<Ui::FlatLabel>(
				ctx.container,
				rpl::single(
					QString("Telegram Desktop v")
					+ QString::fromLatin1(AppVersionStr)),
				st::boxTitle),
			.align = style::al_top,
		};
	});

	builder.addSkip();

	builder.add([](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		return {
			.widget = object_ptr<Ui::FlatLabel>(
				ctx.container,
				rpl::single(QString::fromUtf8("Дополнительные настройки")),
				st::centeredBoxLabel),
			.align = style::al_top,
		};
	});
}

void BuildCategories(SectionBuilder &builder) {
	builder.addSkip();
	builder.addSkip();
	builder.addSkip();
	builder.addSkip();
	builder.addDivider();
	builder.addSkip();

	builder.addSubsectionTitle(tr::ayu_CategoriesHeader());

	builder.addSectionButton({
		.title = rpl::single(QString::fromUtf8("Функции модификации")),
		.targetSection = AyuGhost::Id(),
		.icon = { &st::menuIconGroupReactions },
	});
	builder.addSectionButton({
		.title = tr::ayu_CategoryFilters(),
		.targetSection = AyuFilters::Id(),
		.icon = { &st::menuIconTagFilter },
	});
	builder.addSectionButton({
		.title = tr::ayu_CategoryGeneral(),
		.targetSection = AyuGeneral::Id(),
		.icon = { &st::menuIconShowAll },
	});
	builder.addSectionButton({
		.title = tr::ayu_CategoryAppearance(),
		.targetSection = AyuAppearance::Id(),
		.icon = { &st::menuIconPalette },
	});
	builder.addSectionButton({
		.title = tr::ayu_CategoryChats(),
		.targetSection = AyuChats::Id(),
		.icon = { &st::menuIconChatBubble },
	});
	builder.addSectionButton({
		.title = tr::ayu_CategoryOther(),
		.targetSection = AyuOther::Id(),
		.icon = { &st::menuIconFave },
	});
}

void BuildLinks(SectionBuilder &builder) {
	builder.addSkip();
	builder.addDivider();
	builder.addSkip();

	builder.addSubsectionTitle(tr::ayu_LinksHeader());

	const auto controller = builder.controller();

	builder.addButton({
		.id = u"ayu/channel"_q,
		.title = tr::ayu_LinksChannel(),
		.icon = { &st::menuIconChannel },
		.label = rpl::single(QString("@ayugram")),
		.onClick = [=] {
			controller->showPeerByLink(Window::PeerByLinkInfo{
				.usernameOrId = QString("ayugram"),
			});
		},
	});
	builder.addButton({
		.id = u"ayu/chat"_q,
		.title = tr::ayu_LinksChats(),
		.icon = { &st::menuIconChats },
		.label = rpl::single(QString("@ayugramchat")),
		.onClick = [=] {
			controller->showPeerByLink(Window::PeerByLinkInfo{
				.usernameOrId = QString("ayugramchat"),
			});
		},
	});
	builder.addButton({
		.id = u"ayu/crowdin"_q,
		.title = tr::ayu_LinksTranslate(),
		.icon = { &st::menuIconTranslate },
		.label = rpl::single(QString("Crowdin")),
		.onClick = [=] {
			QDesktopServices::openUrl(
				QString("https://translate.ayugram.one"));
		},
	});
	builder.addButton({
		.id = u"ayu/website"_q,
		.title = tr::ayu_LinksDocumentation(),
		.icon = { &st::menuIconIpAddress },
		.label = rpl::single(QString("docs.ayugram.one")),
		.onClick = [=] {
			QDesktopServices::openUrl(
				QString("https://docs.ayugram.one"));
		},
	});

	builder.addSkip();
}

const auto kMeta = BuildHelper({
	.id = AyuMain::Id(),
	.parentId = MainId(),
	.title = &tr::ayu_AyuPreferences,
	.icon = &st::menuIconPremium,
}, [](SectionBuilder &builder) {
	BuildLogo(builder);
	builder.addSkip();
	BuildVersionInfo(builder);
	BuildCategories(builder);
	BuildLinks(builder);
});

} // namespace

rpl::producer<QString> AyuMain::title() {
	return rpl::single(QString(""));
}

AyuMain::AyuMain(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void AyuMain::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type AyuMainId() {
	return AyuMain::Id();
}

} // namespace Settings
