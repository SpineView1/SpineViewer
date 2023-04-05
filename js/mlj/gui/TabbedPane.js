/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

(function (component) {

    MLJ.gui.TabbedPane = function () {

        var _tabs = [];
        var _$tabbedPane = $('<div id="mlj-tabbed-pane"></div>');
        var _$tabsBar = $('<ul id="mlj-tabs-bar"></ui>');

        var _$filterWrapp = $('<div/>').css({
            overflow: "auto",
            width: "100%"
        });


        var _$rendPane = $('<div/>').css({
            position: "relative"            
        });

        //Accordion for filters pane
        var _filtersAccord = new component.Accordion({
            heightStyle: 'content',
            collapsible: true,
            active: false
        });
        _filtersAccord.$.attr('id', 'accordion-filters');

        //Tool bar for rendering pane
        var _renderingTb = new component.ToolBar();

        function Tab(name) {
            this.name = name;
            var _$content = $('<div id="tab-' + name + '"></div>');

            this.$tab = function () {
                return $('<li><a href="#tab-' + name + '"><span>' + name + '</span></a></li>');
            };

            this.$content = function () {
                return _$content;
            };

            this.appendContent = function (content) {
                _$content.append(content);
                return this;
            };
        }

        function resize() {
            _$tabbedPane.outerHeight(_$tabbedPane.parent().height());

            $("#tab-Filters").outerHeight(
                    _$tabbedPane.height() - _$tabsBar.outerHeight());

            _$filterWrapp.outerHeight($("#tab-Filters").height()
                    - $('#mlj-search-widget').height());

            $("#tab-Rendering").outerHeight(
                    _$tabbedPane.height() - _$tabsBar.outerHeight());

            _$rendPane.outerHeight($("#tab-Rendering").height()
                    - _renderingTb.$.outerHeight());
        }

        function init() {
            _$tabbedPane.append(_$tabsBar);

            var filterTab = new Tab("Filters");
            filterTab
                    .appendContent(MLJ.gui.getWidget("SearchTool")._make())
                    .appendContent(_$filterWrapp);
            _$filterWrapp.append(_filtersAccord.$);

            var renderingTab = new Tab("Rendering");
            renderingTab
                    .appendContent(_renderingTb.$)
                    .appendContent(_$rendPane);

            _tabs.push(filterTab, renderingTab);

            _$tabbedPane.on('tabsactivate', function (event, ui) {
                resize();
            });
        }

        this._make = function () {//build function                            
            $(window).ready(function () {
                var tab;
                for (var i = 0, m = _tabs.length; i < m; i++) {
                    tab = _tabs[i];
                    _$tabsBar.append(tab.$tab);
                    _$tabbedPane.append(tab.$content());
                }

                _$tabbedPane.tabs();

                resize();
            });

            $(window).resize(function () {
                resize();
            });

            return _$tabbedPane;
        };

        this._refresh = function () {
            _$tabbedPane.tabs("refresh");
        };

        this.getFiltersAccord = function () {
            return _filtersAccord;
        };

        this.getRenderingPane = function () {
            return _$rendPane;
        };

        this.getRendToolBar = function () {
            return _renderingTb;
        };
        
        this.selectTab = function(index) {
            _$tabbedPane.tabs("option","active",index);
        };

        init();
    };

    MLJ.extend(MLJ.gui.Widget, MLJ.gui.TabbedPane);

    //Install widget
    MLJ.gui.installWidget("TabbedPane", new MLJ.gui.TabbedPane());

})(MLJ.gui.component);