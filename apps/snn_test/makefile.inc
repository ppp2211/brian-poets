

$(eval $(call provider_rules_template,snn_test))

define snn_test_instance_template	
# $1 = params
# $2 = maxsteps
# $5 = filters

demos/snn_test/snn_test_$1.xml : apps/snn_test/create_sparse_instance.py apps/snn_test/snn_test_graph_type.xml
	mkdir -p $$(dir $$@)
	$$(PYTHON) apps/snn_test/create_sparse_instance.py  $$(subst _, ,$1) > $$@

demos/snn_test/snn_test_$1.snap : demos/snn_test/snn_test_$1.xml bin/epoch_sim providers/snn_test.graph.so demos/snn_test/snn_test_$1.checked
	bin/epoch_sim --stats-delta 1000 --max-steps $2 --log-level 0 --snapshots 1000 $$@  demos/snn_test/snn_test_$1.xml

ALL_SNN_TEST_TESTS += demos/snn_test/snn_test_$1.snap

demos/snn_test/snn_test_$1.gif : demos/snn_test/snn_test_$1.xml demos/snn_test/snn_test_$1.snap
	mkdir -p demos/snn_test/snn_test_$1.tmp
	$(PYTHON) tools/render_graph_as_dot.py demos/snn_test/snn_test_$1.xml --snapshots demos/snn_test/snn_test_$1.snap --output demos/snn_test/snn_test_$1.tmp/out  \
		$3
	(cd demos/snn_test/snn_test_$1.tmp/  &&  for i in *.dot; do echo $$$$i; neato -Gsize=5,5\! -Gdpi=100 -Tpng $$$$i > $$$$i.png; done )
	(cd demos/snn_test/snn_test_$1.tmp/  && convert -delay 20 -loop 0 *.dot.png ../snn_test_$1.gif)
	
ALL_SNN_TEST_DEMOS += demos/snn_test/snn_test_$1.gif
	
endef


SNN_TEST_FILTERS = \
  --bind-dev "neuron" "state" "v" "color" "blend_colors( (255,255,0), (255,0,255), -70, -50, value)"

#   --bind-edge "pulse" "firings" "relative" "color" "blend_colors( (255,0,0,0), (255,0,0,255), 0, 1, value)" 

$(eval $(call snn_test_instance_template,8_2_10,100000,$(SNN_TEST_FILTERS)))

$(eval $(call snn_test_instance_template,50_10_60,10000,$(SNN_TEST_FILTERS)))
$(eval $(call snn_test_instance_template,80_20_100,10000,$(SNN_TEST_FILTERS)))
$(eval $(call snn_test_instance_template,200_50_50,10000,$(SNN_TEST_FILTERS)))


snn_test_tests : $(ALL_SNN_TEST_TESTS)

snn_test_demos : $(ALL_SNN_TEST_DEMOS)

ALL_TESTS += snn_test_tests

ALL_DEMOS += snn_test_demos



