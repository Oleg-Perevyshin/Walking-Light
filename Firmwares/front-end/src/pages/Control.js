// Страница "УПРАВЛЕНИЕ" (центр)

import {h, Component} from "preact";
import Loading from "../components/Loading";
import Message from "../components/Message";
import Button from "../components/Button";
import Select from "../components/Select";
import RangeSingle from "../components/RangeSingle";
import RangeMulti from "../components/RangeMulti";
import {fetchJSON, fetchText} from "../util";
import {stringify} from "querystring";

export default class ControlPage extends Component {
    constructor() {
        super();
        this.state = {
            settings: {}
        };
        this.setParam = this.setParam.bind(this);
    }

    componentWillMount() {
        this.setState({
            lightThreshold: [
                'c.m.generalSettings.lightThreshold.0',
                'c.m.generalSettings.lightThreshold.1',
                'c.m.generalSettings.lightThreshold.2',
                'c.m.generalSettings.lightThreshold.3',
                'c.m.generalSettings.lightThreshold.4',
                'c.m.generalSettings.lightThreshold.5'
            ],
            mainModes: [
                'c.m.globalSettings.mode.0',
                'c.m.globalSettings.mode.1',
                'c.m.globalSettings.mode.2'
            ],
            mainSchemes: [
                'c.m.globalSettings.scheme.0',
                'c.m.globalSettings.scheme.1',
                'c.m.globalSettings.scheme.2',
                'c.m.globalSettings.scheme.3',
                'c.m.globalSettings.scheme.4'
            ],
            standbyTypes: [
                'c.m.edit.standby.typeEffect.0',
                'c.m.edit.standby.typeEffect.1',
                'c.m.edit.standby.typeEffect.2',
                'c.m.edit.standby.typeEffect.3',
                'c.m.edit.standby.typeEffect.4'
            ],
            entryTypes: [
                'c.m.edit.entry.typeEffect.0',
                'c.m.edit.entry.typeEffect.1',
                'c.m.edit.entry.typeEffect.2',
                'c.m.edit.entry.typeEffect.3',
                'c.m.edit.entry.typeEffect.4'
            ],
            followTypes: [
                'c.m.edit.follow.typeEffect.0',
                'c.m.edit.follow.typeEffect.1',
                'c.m.edit.follow.typeEffect.2',
                'c.m.edit.follow.typeEffect.3',
                'c.m.edit.follow.typeEffect.4'
            ],
            exitTypes: [
                'c.m.edit.exit.typeEffect.0',
                'c.m.edit.exit.typeEffect.1',
                'c.m.edit.exit.typeEffect.2',
                'c.m.edit.exit.typeEffect.3',
                'c.m.edit.exit.typeEffect.4'
            ],
            serviceSimulation: [
                'c.m.service.tests.simulation.off',
                'c.m.service.tests.simulation.standby',
                'c.m.service.tests.simulation.entry',
                'c.m.service.tests.simulation.follow',
                'c.m.service.tests.simulation.exit'
            ],
            visualScheme: false,
            edit: false,
            editExp: false,
            preSetting: false,
            standbySpeed: false,
            standbyRangeSin: false,
            standbyRangeMul: false,
            entrySpeed: false,
            entryRangeSin: false,
            entryRangeMul: false,
            followSpeed: false,
            followRangeSin: false,
            followRangeMul: false,
            exitSpeed: false,
            exitRangeSin: false,
            exitRangeMul: false,
            generalSettings: false
        });
        this.getSettings();
        this.onChangeMode = this.onChangeMode.bind(this);
        this.onChangeScheme = this.onChangeScheme.bind(this);
        this.onChangeStandbyType = this.onChangeStandbyType.bind(this);
        this.onChangeEntryType = this.onChangeEntryType.bind(this);
        this.onChangeFollowType = this.onChangeFollowType.bind(this);
        this.onChangeExitType = this.onChangeExitType.bind(this);
    }

    // Получение настроек устройства
    getSettings() {
        return fetchJSON('/mcu/settings')
            .then(settings => {
                this.setState({settings});
            })
            .catch(e => {
                console.error(e);
            });
    }

    // Передача параметров схемы
    setSch(event) {
        let id = event.target.id;
        this.setState({id});
        return fetch(`/mcu/sch?gsh=${id}&` + stringify(this.state.settings))
            .then(() => {
                this.getSettings();
            })
            .catch(e => {
                console.error(e);
            });
    }

    // Передача параметра (id=value)
    setParam(event) {
        this.setState({settings: {...this.state.settings, [event.target.id]: event.target.value}});
        this.context.sendWS(event);
    }

    onChangeMode(event) {
        this.setParam(event);
        switch (event.target.value) {
            case '0':
                this.setState({visualScheme: false});
                this.setState({preSetting: false});
                this.setState({edit: false});
                this.setState({generalSettings: true});
                break;
            case '1':
                this.setState({visualScheme: true});
                this.setState({edit: true});
                this.setState({generalSettings: false});
                break;
            case '2':
                this.setState({visualScheme: true});
                this.setState({edit: false});
                this.setState({generalSettings: false});
                break;
            default:
                this.setState({standbySpeed: false});
                this.setState({preSetting: false});
                this.setState({edit: false});
                this.setState({generalSettings: false});
        }
    }

    onChangeScheme(event) {
        this.setParam(event);
        switch (event.target.value) {
            case '0':
                this.setState({preSetting: true});
                this.setState({edit: false});
                this.setState({generalSettings: false});
                break;
            case '1':
                this.setState({preSetting: true});
                this.setState({edit: false});
                this.setState({generalSettings: false});
                break;
            case '2':
                this.setState({preSetting: true});
                this.setState({edit: false});
                this.setState({generalSettings: false});
                break;
            case '3':
                this.setState({preSetting: false});
                this.setState({edit: true});
                this.setState({generalSettings: false});
                break;
            case '4':
                this.setState({preSetting: false});
                this.setState({edit: true});
                this.setState({generalSettings: false});
                break;
            default:
                this.setState({preSetting: true});
                this.setState({edit: false});
                this.setState({generalSettings: false});
        }
    }

    onChangeStandbyType(event) {
        this.setParam(event);
        this.linkState('stp');
        switch (event.target.value) {
            case '0':
                this.setState({standbySpeed: false});
                this.setState({standbyRangeSin: false});
                this.setState({standbyRangeMul: false});
                break;
            case '1':
                this.setState({standbySpeed: false});
                this.setState({standbyRangeSin: true});
                this.setState({standbyRangeMul: false});
                break;
            case '2':
                this.setState({standbySpeed: true});
                this.setState({standbyRangeSin: false});
                this.setState({standbyRangeMul: true});
                break;
            case '3':
                this.setState({standbySpeed: true});
                this.setState({standbyRangeSin: false});
                this.setState({standbyRangeMul: true});
                break;
            case '4':
                this.setState({standbySpeed: true});
                this.setState({standbyRangeSin: true});
                this.setState({standbyRangeMul: false});
                break;
            default:
                this.setState({standbySpeed: false});
                this.setState({standbyRangeSin: false});
                this.setState({standbyRangeMul: false});
        }
    }

    onChangeEntryType(event) {
        this.setParam(event);
        switch (event.target.value) {
            case '0':
                this.setState({entrySpeed: false});
                this.setState({entryRangeSin: false});
                this.setState({entryRangeMul: false});
                break;
            case '1':
                this.setState({entrySpeed: true});
                this.setState({entryRangeSin: true});
                this.setState({entryRangeMul: false});
                break;
            case '2':
                this.setState({entrySpeed: true});
                this.setState({entryRangeSin: true});
                this.setState({entryRangeMul: false});
                break;
            case '3':
                this.setState({entrySpeed: true});
                this.setState({entryRangeSin: false});
                this.setState({entryRangeMul: true});
                break;
            case '4':
                this.setState({entrySpeed: true});
                this.setState({entryRangeSin: false});
                this.setState({entryRangeMul: true});
                break;
            default:
                this.setState({entrySpeed: false});
                this.setState({entryRangeSin: false});
                this.setState({entryRangeMul: false});
        }
    }

    onChangeFollowType(event) {
        this.setParam(event);
        switch (event.target.value) {
            case '0':
                this.setState({followSpeed: false});
                this.setState({followRangeSin: false});
                this.setState({followRangeMul: false});
                break;
            case '1':
                this.setState({followSpeed: true});
                this.setState({followRangeSin: true});
                this.setState({followRangeMul: false});
                break;
            case '2':
                this.setState({followSpeed: true});
                this.setState({followRangeSin: true});
                this.setState({followRangeMul: false});
                break;
            case '3':
                this.setState({followSpeed: true});
                this.setState({followRangeSin: true});
                this.setState({followRangeMul: false});
                break;
            case '4':
                this.setState({followSpeed: true});
                this.setState({followRangeSin: true});
                this.setState({followRangeMul: false});
                break;
            default:
                this.setState({followSpeed: false});
                this.setState({followRangeSin: false});
                this.setState({followRangeMul: false});
        }
    }

    onChangeExitType(event) {
        this.setParam(event);
        switch (event.target.value) {
            case '0':
                this.setState({exitSpeed: false});
                this.setState({exitRangeSin: false});
                this.setState({exitRangeMul: false});
                break;
            case '1':
                this.setState({exitSpeed: true});
                this.setState({exitRangeSin: true});
                this.setState({exitRangeMul: false});
                break;
            case '2':
                this.setState({exitSpeed: true});
                this.setState({exitRangeSin: true});
                this.setState({exitRangeMul: false});
                break;
            case '3':
                this.setState({exitSpeed: true});
                this.setState({exitRangeSin: true});
                this.setState({exitRangeMul: false});
                break;
            case '4':
                this.setState({exitSpeed: true});
                this.setState({exitRangeSin: false});
                this.setState({exitRangeMul: true});
                break;
            default:
                this.setState({exitSpeed: false});
                this.setState({exitRangeSin: false});
                this.setState({exitRangeMul: false});
        }
    }


    render({}, {lightThreshold, mainModes, mainSchemes, standbyTypes, entryTypes, followTypes, exitTypes, serviceSimulation}) {
        return (
            <div>
                <h2 className="tPan"><Message key="c.m.t"/></h2>
                {this.state.loading ? <Loading/> : (
                    <div>
                        {/* ОСНОВНЫЕ ПАРАМЕТРЫ */}
                        <div className="sb">
                            <p className="tsb"><Message key="c.m.globalSettings.t"/></p>
                            <div className="spl">
                                {/* Режим работы */}
                                <p className="bold"><Message key="c.m.globalSettings.mode.t"/></p>
                                {mainModes.map(item =>
                                    <Button id="gmd" className={"button w30 red large" +
                                    (mainModes.indexOf(item) === parseInt(this.state.settings.gmd) ? ' active' : '')
                                    }
                                            onClick={this.onChangeMode}
                                            value={mainModes.indexOf(item)}>
                                        <Message key={item}/>
                                    </Button>
                                )}
                                {/* Визуальные схемы */}
                                <div className={(this.state.visualScheme ? '' : ' hidden')}>
                                    <p className="bold"><Message key="c.m.globalSettings.scheme.t"/></p>
                                    {mainSchemes.map(item =>
                                        <Button id="gsh" className={"button w30 blue large" +
                                        (mainSchemes.indexOf(item) === parseInt(this.state.settings.gsh) ? ' active' : '')
                                        }
                                                onClick={this.onChangeScheme}
                                                value={mainSchemes.indexOf(item)}>
                                            <Message key={item}/>
                                        </Button>
                                    )}
                                </div>
                                {/* Скорость предустановленных эффектов */}
                                <div className={(this.state.preSetting ? 'block-in' : ' hidden')}>
                                    <p className="bold"><Message
                                        key="c.m.speedEffects"/></p>
                                    <RangeSingle id="gsp" className="w75" min="0" max="15" step="1"
                                                 value={this.state.settings.gsp}
                                                 onChange={this.setParam}/>
                                    {/* Яркость предустановленных эффектов */}
                                    <p className="bold"><Message key="c.m.staticBright"/></p>
                                    <RangeSingle id="glb" className="w75" min="0" max="15" step="1"
                                                 value={this.state.settings.glb}
                                                 onChange={this.setParam}/>
                                </div>
                            </div>
                        </div>

                        {/* РЕДАКТОР СХЕМ */}
                        <div className={(this.state.edit ? '' : ' hidden')}>
                            <div className="sb">
                                <input type="checkbox" id="edit" className="checker"
                                       checked={this.state.editExp}
                                       onChange={this.linkState('editExp')}/>
                                <label for="edit"><p className="tsb"><Message
                                    key="c.m.edit.t"/>
                                </p></label>
                                <div className="spl">
                                    {/* ДЕЖУРНАЯ ПОДСВЕТКА */}
                                    <div className="block-in">
                                        <h2 className="bold title-editscheme"><Message
                                            key="c.m.edit.standby.t"/></h2>
                                        {/* Тип эффекта */}
                                        <p className="bold"><Message key="c.m.edit.effect"/></p>
                                        <Select id="stp" className="select w75 large"
                                                value={this.state.settings.stp}
                                                onChange={this.onChangeStandbyType}>
                                            {standbyTypes.map(item =>
                                                <option value={standbyTypes.indexOf(item)}><Message key={item}/>
                                                </option>
                                            )}
                                        </Select>
                                        {/* Скорость эффекта */}
                                        <div className={(this.state.standbySpeed ? '' : ' hidden')}>
                                            <p className="bold"><Message
                                                key="c.m.edit.speedEffects"/></p>
                                            <RangeSingle id="ssp" className="w75" min="0" max="15" step="1"
                                                         value={this.state.settings.ssp}
                                                         onChange={this.setParam}/>
                                        </div>
                                        {/* Минимальная и максимальная яркости */}
                                        <div className={(this.state.standbyRangeMul ? '' : ' hidden')}>
                                            <p className="bold"><Message key="c.m.rangeBright"/></p>
                                            <RangeMulti className="w75" multiple min="0" max="15" step="1"
                                                        ids={["slb", "sub"]}
                                                        values={[this.state.settings.slb, this.state.settings.sub]}
                                                        onChange={this.setParam}/>
                                        </div>
                                        {/* Постоянная яркость */}
                                        <div className={(this.state.standbyRangeSin ? '' : ' hidden')}>
                                            <p className="bold"><Message key="c.m.staticBright"/></p>
                                            <RangeSingle id="slb" className="w75" min="0" max="15" step="1"
                                                         value={this.state.settings.slb}
                                                         onChange={this.setParam}/>
                                        </div>
                                    </div>
                                    {/* РЕЖИМ "ВХОД" */}
                                    <div className="block-in">
                                        <h2 className="bold title-editscheme"><Message
                                            key="c.m.edit.entry.t"/></h2>
                                        {/* Тип эффекта */}
                                        <p className="bold"><Message key="c.m.edit.effect"/></p>
                                        <Select id="itp" className="select w75 large"
                                                value={this.state.settings.itp}
                                                onChange={this.onChangeEntryType}>
                                            {entryTypes.map(item =>
                                                <option value={entryTypes.indexOf(item)}><Message key={item}/>
                                                </option>
                                            )}
                                        </Select>
                                        {/* Скорость эффекта */}
                                        <div className={(this.state.entrySpeed ? '' : ' hidden')}>
                                            <p className="bold"><Message
                                                key="c.m.edit.speedEffects"/></p>
                                            <RangeSingle id="isp" className="w75" min="0" max="15" step="1"
                                                         value={this.state.settings.isp}
                                                         onChange={this.setParam}/>
                                        </div>
                                        {/* Минимальная и максимальная яркости */}
                                        <div className={(this.state.entryRangeMul ? '' : ' hidden')}>
                                            <p className="bold"><Message key="c.m.rangeBright"/></p>
                                            <RangeMulti className="w75" multiple min="0" max="15" step="1"
                                                        ids={["ilb", "iub"]}
                                                        values={[this.state.settings.ilb, this.state.settings.iub]}
                                                        onChange={this.setParam}/>
                                        </div>
                                    </div>
                                    {/* РЕЖИМ "СОПРОВОЖДЕНИЕ" */}
                                    <div className="block-in">
                                        <h2 className="bold title-editscheme"><Message
                                            key="c.m.edit.follow.t"/></h2>
                                        {/* Тип эффекта */}
                                        <p className="bold"><Message key="c.m.edit.effect"/></p>
                                        <Select id="ftp" className="select w75 large"
                                                value={this.state.settings.ftp}
                                                onChange={this.onChangeFollowType}>
                                            {followTypes.map(item =>
                                                <option value={followTypes.indexOf(item)}><Message key={item}/>
                                                </option>
                                            )}
                                        </Select>
                                        {/* Скорость эффекта */}
                                        <div className={(this.state.followSpeed ? '' : ' hidden')}>
                                            <p className="bold"><Message
                                                key="c.m.edit.speedEffects"/>
                                            </p>
                                            <RangeSingle id="fsp" className="w75" min="0" max="15" step="1"
                                                         value={this.state.settings.fsp}
                                                         onChange={this.setParam}/>
                                        </div>
                                        {/* Минимальная и максимальная яркости */}
                                        <div className={(this.state.followRangeMul ? '' : ' hidden')}>
                                            <p className="bold"><Message
                                                key="c.m.edit.follow.mulBright"/></p>
                                            <RangeMulti className="w75" multiple min="0" max="15" step="1"
                                                        ids={["flb", "fub"]}
                                                        values={[this.state.settings.flb, this.state.settings.fub]}
                                                        onChange={this.setParam}/>
                                        </div>
                                    </div>
                                    {/* РЕЖИМ "ВЫХОД" */}
                                    <div className="block-in">
                                        <h2 className="bold title-editscheme"><Message
                                            key="c.m.edit.exit.t"/></h2>
                                        {/* Тип эффекта */}
                                        <p className="bold"><Message key="c.m.edit.effect"/></p>
                                        <Select id="otp" className="select w75 large"
                                                value={this.state.settings.otp}
                                                onChange={this.onChangeExitType}>
                                            {exitTypes.map(item =>
                                                <option value={exitTypes.indexOf(item)}><Message key={item}/>
                                                </option>
                                            )}
                                        </Select>
                                        {/* Скорость эффекта */}
                                        <div className={(this.state.exitSpeed ? '' : ' hidden')}>
                                            <p className="bold"><Message
                                                key="c.m.edit.speedEffects"/></p>
                                            <RangeSingle id="osp" className="w75" min="0" max="15" step="1"
                                                         value={this.state.settings.osp}
                                                         onChange={this.setParam}/>
                                        </div>
                                        {/* Минимальная и максимальная яркости */}
                                        <div className={(this.state.exitRangeMul ? '' : ' hidden')}>
                                            <p className="bold"><Message key="c.m.rangeBright"/></p>
                                            <RangeMulti className="w75" multiple min="0" max="15" step="1"
                                                        ids={["olb", "oub"]}
                                                        values={[this.state.settings.olb, this.state.settings.oub]}
                                                        onChange={this.setParam}/>
                                        </div>
                                    </div>
                                    <p className="bold"><Message key="c.m.edit.tSave"/></p>
                                    <Button id="s1" className="button w75 blue large"
                                            onClick={this.setSch.bind(this)}><Message
                                        key="c.m.edit.saveScheme_1"/></Button>
                                    <Button id="s2" className="button w75 blue large"
                                            onClick={this.setSch.bind(this)}><Message
                                        key="c.m.edit.saveScheme_2"/></Button>
                                </div>
                            </div>
                        </div>

                        {/* ОБЩИЕ НАСТРОЙКИ */}
                        <div className={(this.state.generalSettings ? '' : ' hidden')}>
                            <div className="sb">
                                <p className="tsb"><Message key="c.m.generalSettings.t"/></p>
                                {/* Количество активных каналов */}
                                <p className="bold"><Message key="c.m.generalSettings.activeChannels"/></p>
                                <RangeSingle id="gac" className="w75" min="8" max="24" step="1"
                                             value={this.state.settings.gac}
                                             onChange={this.setParam}/>
                                {/* Порог уровня освещенности */}
                                <p className="bold"><Message
                                    key="c.m.generalSettings.lightThreshold.t"/>
                                </p>
                                <Select id="glt" className="select w75 large" value={this.state.settings.glt}
                                        onChange={this.setParam}>
                                    {lightThreshold.map(item =>
                                        <option value={lightThreshold.indexOf(item)}><Message key={item}/></option>
                                    )}
                                </Select>
                                {/* Количество циклов до отключения */}
                                <p className="bold"><Message key="c.m.generalSettings.cyclesProtect"/></p>
                                <RangeSingle id="gcs" className="w75" min="3" max="25" step="1"
                                             value={this.state.settings.gcs}
                                             onChange={this.setParam}/>
                                {/* Период запуска имитации присутствия */}
                                <p className="bold"><Message key="c.m.generalSettings.periodSimulation"/>
                                </p>
                                <RangeSingle id="gpi" className="w75" min="5" max="250" step="5"
                                             value={this.state.settings.gpi}
                                             onChange={this.setParam}/>
                            </div>
                        </div>
                    </div>

                )}
            </div>
        );
    }
}
