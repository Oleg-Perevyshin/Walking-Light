import {h, Component} from "preact";
import {fetchJSON, translate} from "../util";
import Button from "./Button";
import Input from "./Input";
import Select from "./Select";
import Message from "./Message";
import Loading from "./Loading";

const EVENTS = 0;
const ACTIONS = 1;

const DeviceList = ({devices, selectedDevice, onChange}) => (
    <Select className="flex-elem fe1 select small"
            style={`border: 1px solid #${devices[selectedDevice.da].dc}`}
            onChange={onChange} value={selectedDevice.da}>
        {
            Object.keys(devices).map(key => {
                let {da, dm, dn} = devices[key];
                return <option value={da}>{`${dm}: ${dn}`}</option>
            })
        }
    </Select>
);

const DeviceAPI = ({type, api, dictionary, selectedData, deletable, onCategoryChange, onSubcategoryChange, onAddButtonClick, onRemoveButtonClick, color}) => {
    const [selectedCategory, selectedSubcategory] = selectedData;
    return (
        <div className="flex-cont">
            <Select className="flex-elem fe4 select small" style={`border: 1px solid #${color};`}
                    onChange={onCategoryChange}
                    value={selectedCategory}>
                {api.map((_, i) =>
                    <option value={i}>{translate(dictionary, `api.${type}.${i}.name`)}</option>
                )}
            </Select>
            <Select className="flex-elem fe5 select small" style={`border: 1px solid #${color}`}
                    onChange={onSubcategoryChange}
                    value={selectedSubcategory}>
                {new Array(api[selectedCategory]).fill(null).map((_, i) => {
                    return <option
                        value={i}>{translate(dictionary, `api.${type}.${selectedCategory}.options.${i}`)}</option>
                })}
            </Select>
            <Button className="flex-elem fe6 button e2 gray small" onClick={deletable && onRemoveButtonClick}>-</Button>
            <Button className="flex-elem fe7 button e2 blue small" onClick={onAddButtonClick}>+</Button>
            <Button className="flex-elem fe8 button e2 empty small"/>
        </div>
    );
};

const loadDeviceData = ip => {
    return (
        fetchJSON(`//${ip}/config.json`)
            .then(function (json) {
                const device = {ip: ip, status: 'loading', ...json};
                return fetchJSON(`//${ip}/mcu/api`)
                    .then(api => {
                        device.api = api;
                        return fetchJSON(`//${ip}/lang?dl=${json.dl}.json`)
                            .then(dl => {
                                device.dictionary = dl;
                                device.status = 'ok';
                                return device;
                            })
                    })
            })
            .catch((e) => ( {ip: ip, status: 'error'}))
    );
};

class Scripts extends Component {
    constructor() {
        super();
        this.setState({
            scripts: [],
            devices: {},
            loading: true
        });
    }

    componentWillMount() {
        this.setState({loading: true});
        this.context.devices.map((item, index) => {
            if (index === 0) {
                this.setState({defaultDevice: item.da});
            }
            loadDeviceData(item.ip)
                .then(data => {
                    let {devices} = this.state;
                    devices[item.da] = data;
                    this.setState({devices});
                    return fetchJSON('/mcu/scr')
                })
                .then(data => this.setState({
                    scripts: data.map(script => {
                        script.collapsed = false;
                        return script;
                    })
                }))
                .catch(e => {
                    this.setState({scripts: []});
                    console.error(e);
                })
                .then(() => this.setState({loading: false}));
        });
    }

    // Удалить текущий сценарий
    removeScript(index) {
        const {scripts} = {...this.state};
        scripts.splice(index, 1);
        this.setState({scripts});
    }

    // Удалить все сценарии (передаем пустой массив)
    removeScripts() {
        this.setState({scripts: []});
        this.saveScripts();
    }

    // Создать сценарий
    createScript() {
        const {scripts, devices, defaultDevice} = {...this.state};
        const localDevice = devices[defaultDevice];
        scripts.push({
            collapsed: false,
            name: `${translate(this.context.dictionary, 'sc.m.new')} (${scripts.length})`,
            enabled: false,
            custom_param: '',
            events: [{da: localDevice.da, params: [[0, 0]]}],
            actions: [{da: localDevice.da, params: [[0, 0]]}]
        });
        this.setState({scripts});
    }

    // Сохранение сценариев
    saveScripts() {
        this.setState({loading: true});
        const {scripts} = this.state;
        fetch('/mcu/scw', {
            method: 'POST',
            body: JSON.stringify(scripts.map(({name, enabled, custom_param, events, actions}, id) => ({
                id, name, enabled, custom_param, events, actions
            })))
        })
            .then(() => this.setState({loading: false}))
            .catch(e => console.error(`Failed saving scripts: ${e}`))
    }

    onNameInput(index, event) {
        const {scripts} = {...this.state};
        scripts[index].name = event.target.value;
        this.setState({scripts});
    }

    onCustomParamInput(index, event) {
        const {scripts} = {...this.state};
        scripts[index].custom_param = event.target.value.toUpperCase().replace(/[^0-9]/g, '').substring(0, 4);
        this.setState({scripts});
    }

    toggleActiveState(index) {
        const {scripts} = {...this.state};
        scripts[index].enabled = +!scripts[index].enabled;
        this.setState({scripts});
    }

    addDevice(category, scriptIndex, after) {
        const {scripts, devices, defaultDevice} = {...this.state};
        const device = devices[defaultDevice];
        const {da} = device;
        let target;
        let data;
        if (category === EVENTS) {
            target = scripts[scriptIndex].events;
            data = {da, params: [[0, 0]]};
        } else if (category === ACTIONS) {
            target = scripts[scriptIndex].actions;
            data = {da, params: [[0, 0]]};
        }
        if (target) {
            target.splice(++after, 0, data);
            this.setState({scripts});
        } else
            console.error('Error Adding Device');
    }

    removeDevice(category, scriptIndex, selectedDeviceIndex) {
        const {scripts} = {...this.state};
        let target;
        if (category === EVENTS)
            target = scripts[scriptIndex].events;
        else if (category === ACTIONS)
            target = scripts[scriptIndex].actions;
        if (target) {
            target.splice(selectedDeviceIndex, 1);
            this.setState({scripts});
        } else
            console.error('Error Removing Device');
    }

    onDeviceChange(category, scriptIndex, selectedDeviceIndex, e) {
        const {scripts, devices} = {...this.state};
        const device = devices[e.target.value];
        let target = category === EVENTS ? scripts[scriptIndex].events : scripts[scriptIndex].actions;
        let data = [0, 0];
        if (target) {
            target[selectedDeviceIndex] = {
                da: device.da,
                params: [data]
            };
            this.setState({scripts});
        } else
            console.error('Error Changing Device');
    }

    onCategoryChange(category, scriptIndex, selectedDeviceIndex, selectedIndex, e) {
        const {scripts} = {...this.state};
        let target = category === EVENTS ?
            scripts[scriptIndex].events[selectedDeviceIndex].params :
            scripts[scriptIndex].actions[selectedDeviceIndex].params;
        let data = [+e.target.value, 0];
        if (target) {
            target[selectedIndex] = data;
            this.setState({scripts});
        } else
            console.error('Error Changing Category');
    }

    onSubcategoryChange(category, scriptIndex, selectedDeviceIndex, selectedIndex, e) {
        const {scripts} = {...this.state};
        let target = category === EVENTS ?
            scripts[scriptIndex].events[selectedDeviceIndex].params :
            scripts[scriptIndex].actions[selectedDeviceIndex].params;
        if (target) {
            target[selectedIndex][1] = +e.target.value;
            this.setState({scripts});
        } else
            console.error('Error Changing Subcategory');
    }

    onAddButtonClick(category, scriptIndex, selectedDeviceIndex, after) {
        const {scripts} = {...this.state};
        let target = category === EVENTS ?
            scripts[scriptIndex].events[selectedDeviceIndex].params :
            scripts[scriptIndex].actions[selectedDeviceIndex].params;
        let data = [0, 0];
        if (target) {
            target.splice(++after, 0, data);
            this.setState({scripts});
        } else
            console.error('Error Adding');
    }

    onRemoveButtonClick(category, scriptIndex, selectedDeviceIndex, selectedIndex) {
        const {scripts} = {...this.state};
        let target = category === EVENTS ?
            scripts[scriptIndex].events[selectedDeviceIndex].params :
            scripts[scriptIndex].actions[selectedDeviceIndex].params;
        if (target) {
            target.splice(selectedIndex, 1);
            this.setState({scripts});
        } else
            console.error('Error Removing');
    }

    toggleCollapseScript(scriptIndex, event) {
        const {scripts} = {...this.state};
        scripts[scriptIndex].collapsed = !event.target.checked;
        this.setState({scripts});
    }

    render() {
        const {devices, scripts} = {...this.state};
        return this.state.loading ? <Loading/> : (
            <div>
                <div><Button className="button w70 grey large" onClick={::this.createScript}><Message
                    key="sc.m.create"/></Button></div>
                <div><Button className="button w70 red large" onClick={::this.removeScripts}><Message
                    key="sc.m.clear"/></Button></div>
                {
                    scripts && scripts.map((script, scriptIndex) => (
                        <div className="sb">
                            <div className="sc-main">

                                <input type="checkbox" className="switch-hor" id={'switch-hor' + scriptIndex}
                                       onChange={this.toggleActiveState.bind(this, scriptIndex)}
                                       checked={script.enabled}/>
                                <label for={'switch-hor' + scriptIndex} className="sc-switch"/>

                                <Input className="sc-name w75" type="text" value={script.name} maxlength="16"
                                       onInput={this.onNameInput.bind(this, scriptIndex)}>{script.name}</Input>

                                <div className="icon-delete"><Button className="delete"
                                                                     onClick={this.removeScript.bind(this, scriptIndex)}/>
                                </div>

                                <input type="checkbox" id={'sc-collapse' + scriptIndex} className="checker"
                                       onChange={this.toggleCollapseScript.bind(this, scriptIndex)}
                                       checked={script.collapsed !== true}/>
                                <label for={'sc-collapse' + scriptIndex} className="toggle"/>

                                <div className="spl">
                                    <h3 className="bold tsc"><Message key="sc.m.events"/></h3>
                                    <div>
                                        {
                                            script.events.map((selectedDevice, selectedDeviceIndex) => (
                                                (devices.hasOwnProperty(selectedDevice.da) && (devices[selectedDevice.da].status === 'ok')) ?
                                                    (<div className="sc-device">
                                                        <div className="flex-cont">
                                                            <DeviceList devices={devices}
                                                                        selectedDevice={selectedDevice}
                                                                        onChange={this.onDeviceChange.bind(this, EVENTS, scriptIndex, selectedDeviceIndex)}/>
                                                            <Button className="flex-elem fe2 button e2 gray small"
                                                                    onClick={(script.events.length > 1) && this.removeDevice.bind(this, EVENTS, scriptIndex, selectedDeviceIndex)}>-</Button>
                                                            <Button className="flex-elem fe3 button e2 blue small"
                                                                    onClick={this.addDevice.bind(this, EVENTS, scriptIndex, selectedDeviceIndex)}>+</Button>
                                                        </div>
                                                        {
                                                            selectedDevice.params.map((selected, index) => {
                                                                const device = devices[selectedDevice.da];
                                                                return (
                                                                    <DeviceAPI
                                                                        type={EVENTS}
                                                                        color={device.dc}
                                                                        api={device.api[EVENTS]}
                                                                        deletable={selectedDevice.params.length > 1}
                                                                        dictionary={device.dictionary}
                                                                        selectedData={selected}
                                                                        onCategoryChange={this.onCategoryChange.bind(this, EVENTS, scriptIndex, selectedDeviceIndex, index)}
                                                                        onSubcategoryChange={this.onSubcategoryChange.bind(this, EVENTS, scriptIndex, selectedDeviceIndex, index)}
                                                                        onAddButtonClick={this.onAddButtonClick.bind(this, EVENTS, scriptIndex, selectedDeviceIndex, index)}
                                                                        onRemoveButtonClick={this.onRemoveButtonClick.bind(this, EVENTS, scriptIndex, selectedDeviceIndex, index)}
                                                                    />
                                                                )
                                                            })
                                                        }

                                                    </div>) : <Loading/>
                                            ))
                                        }
                                    </div>
                                    <br/>
                                    <h3 className="bold tsc"><Message key="sc.m.a"/></h3>
                                    <div>
                                        {
                                            script.actions.map((selectedDevice, selectedDeviceIndex) => (
                                                (devices.hasOwnProperty(selectedDevice.da) && (devices[selectedDevice.da].status === 'ok')) ?
                                                    (<div className="sc-device">
                                                        <div className="flex-cont">
                                                            <DeviceList devices={devices}
                                                                        selectedDevice={selectedDevice}
                                                                        onChange={this.onDeviceChange.bind(this, ACTIONS, scriptIndex, selectedDeviceIndex)}/>
                                                            <Button className="flex-elem fe2 button e2 gray small"
                                                                    onClick={(script.actions.length > 1) && this.removeDevice.bind(this, ACTIONS, scriptIndex, selectedDeviceIndex)}>-</Button>
                                                            <Button className="flex-elem fe3 button e2 blue small"
                                                                    onClick={this.addDevice.bind(this, ACTIONS, scriptIndex, selectedDeviceIndex)}>+</Button>
                                                        </div>
                                                        {
                                                            selectedDevice.params.map((selected, index) => {
                                                                const device = devices[selectedDevice.da];
                                                                return (
                                                                    <DeviceAPI
                                                                        type={ACTIONS}
                                                                        color={device.dc}
                                                                        api={device.api[ACTIONS]}
                                                                        dictionary={device.dictionary}
                                                                        deletable={selectedDevice.params.length > 1}
                                                                        selectedData={selected}
                                                                        onCategoryChange={this.onCategoryChange.bind(this, ACTIONS, scriptIndex, selectedDeviceIndex, index)}
                                                                        onSubcategoryChange={this.onSubcategoryChange.bind(this, ACTIONS, scriptIndex, selectedDeviceIndex, index)}
                                                                        onAddButtonClick={this.onAddButtonClick.bind(this, ACTIONS, scriptIndex, selectedDeviceIndex, index)}
                                                                        onRemoveButtonClick={this.onRemoveButtonClick.bind(this, ACTIONS, scriptIndex, selectedDeviceIndex, index)}
                                                                    />
                                                                )
                                                            })
                                                        }
                                                    </div>) : <Loading/>
                                            ))
                                        }
                                    </div>
                                    {/* Дополнительный параметр */}
                                    <input type="checkbox" id={'sc-param' + scriptIndex} className="sc-checker"/>
                                    <label for={'sc-param' + scriptIndex} className="sc-toggle"/>
                                    <div className="sc-cont">
                                        <p className="bold"><Message key="sc.m.sc-param"/></p>
                                        <div>
                                            <Input className="input w75 large" type="text"
                                                   value={script.custom_param}
                                                   onInput={this.onCustomParamInput.bind(this, scriptIndex)}/>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    ))
                }
                <Button className="button w70 green large" onClick={::this.saveScripts}><Message
                    key="a.save"/></Button>
            </div>
        )
    }
}

export default Scripts;
