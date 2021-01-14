target_include_directories(app PRIVATE common)

target_sources(app PRIVATE
	common/host.c
	common/keyboard.c
	common/action.c
	common/action_tapping.c
	common/action_macro.c
	common/action_layer.c
	common/action_util.c
	common/print.c
	common/debug.c
	common/sendchar_null.c
	common/util.c
	common/report.c
	common/magic.c
	common/eeconfig.c
)
